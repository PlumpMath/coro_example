#include "retransmitter.h"

#include "repo.h"
#include "sender.h"
#include "receiver.h"
#include "async_operation.h"
#include "tls_cc_keeper.h"

#include <string>
#include <boost/asio/spawn.hpp>

template <typename RetType>
RetType wrap_future(yield_context_ptr& yield, future::future<RetType> fu)
{
    /* [1]
     * В этот момент нам необходимо сохранить callee т.к. yield содержит weak_ptr<caller>
     * и как только мы передадим упралвение назад вызывающему (yield->ca_())
     * вызывающий рарзушит свой объект shared_ptr<caller>, но нам его нужно сохранить
     * так как именно через него мы востановим текущую функцию когда отработает асинхронная
     *
     * Мы не записываем его в tls т.к. мы не выполняем задачу сейчас,
     * мы ставим ее в очредь и не известно в каком треде и когда она выполнится
     */
    callee_type_ptr callee = yield->coro_.lock();

    fu.add_callback([yield]()
    {
        // Ставим задачу разморозки корутины в strand этой корутины
        yield->handler_.dispatcher_.post([yield]()
        {
            /* [4]
             * Восстанавливаем в tls наш контекст
             * Увеличиваем счетчик ссылок на callee.
             * [1] нам гарантирует что объект callee все еще существует
             */
            tls_cc_keeper::set(yield);
            callee_type_ptr callee = yield->coro_.lock();

            /* [5]
             * Передаем управление в [3]
             */
            (*callee)();

            /* [6]
             * Сюда мы попадаем в двух случаях:
             *  1) если корутина передела управление
             *  2) если корутина завершилась
             *
             * Соотвественно, в первом случае разрушение callee не приводит
             * к разрушению самого объекта, т.к. уже был выполнен [1].
             * Во втором случае разрушение callee разрушает и сам объект,
             * так как корутина полностью завершается.
             */
        });
    });

    /* [2]
     * Перед тем как отпустить поток управления (а значит и tls) мы должны удалить из tls
     * упоминания о себе, что бы следущий кто запустится в этом потоке по ошибке не принял наш контекст
     * за тот который ему передал его вызывающий.
     */
    tls_cc_keeper::reset();

    /* [3]
     * ждем когда выполнится [5]
     */
    yield->ca_();

    return fu.get();

    /* [6]
     * Разрушение текущего callee_ptr не приводит к разрушению самого объекта callee,
     * потому что счетчик сылок на него был увелечен в [4] перед передачей управление сюда
     */
}

void wrap_future(yield_context_ptr& yield, future::future<void> fu)
{
    callee_type_ptr callee = yield->coro_.lock();

    fu.add_callback([yield]()
    {
        yield->handler_.dispatcher_.post([yield]()
        {
            tls_cc_keeper::set(yield);
            callee_type_ptr callee = yield->coro_.lock();
            (*callee)();
        });
    });

    tls_cc_keeper::reset();
    yield->ca_();
}

template <typename RetType, typename CoroFunction>
future::future<RetType> path_selection(CoroFunction coro_function)
{
    /* [1]
     * Контекст уже был создан для нас, либо вызывающим, либо был восстановлен
     * по завершению асинхронной функции вызванной из корутины
     */
    if(tls_cc_keeper::is_exist()) // coro way
    {
        yield_context_ptr yield = tls_cc_keeper::get();
        return coro_function(yield);
    }


    /* [2]
     * Вызывающий ничего не знает про корутины, поэтому мы сами ее создаем
     * А для него прикидываемся асинхронной функцией
     */
    future::promise<RetType> prom;
    boost::asio::spawn(active_queue::inst().get_io(),
            [prom, coro_function](boost::asio::yield_context yield) mutable
            {
                yield_context_ptr yield_ptr = tls_cc_keeper::make_yield_context(yield);
                prom.set(coro_function(yield_ptr).get());

                /* Перед тем как отпустить поток управлениея, удалим из tls
                 * наш контекст
                 */
                tls_cc_keeper::reset();
            });
    return prom;
}

template <typename CoroFunction>
future::future<void> path_selection(CoroFunction coro_function)
{
    if(tls_cc_keeper::is_exist())
    {
        yield_context_ptr yield = tls_cc_keeper::get();
        coro_function(yield);
        future::promise<void> prom; prom.set();
        return prom;
    }

    future::promise<void> prom;
    boost::asio::spawn(active_queue::inst().get_io(),
            [prom, coro_function](boost::asio::yield_context yield) mutable
            {
                yield_context_ptr yield_ptr = tls_cc_keeper::make_yield_context(yield);
                coro_function(yield_ptr);
                prom.set();
                tls_cc_keeper::reset();
            });
    return prom;
}

void retransmitter_impl::retransmit_impl(yield_context_ptr& yield, int uniq_iq)
{
    std::stringstream planet;
    planet << "Saturn" << uniq_iq;

    std::string msg = wrap_future(yield, MODULE(receiver).recv(planet.str()));
    wrap_future(yield, MODULE(sender).send(msg));
}

complete_future retransmitter_impl::retransmit (int uniq_iq)
{
    return path_selection(boost::bind(
            &retransmitter_impl::retransmit_impl, this, _1, uniq_iq));
}

