#ifndef MODULE_INTERFACE_H_
#define MODULE_INTERFACE_H_


class module_interface
{
public:
    virtual char const* name () const = 0;
    virtual ~module_interface(){}
};


#endif /* MODULE_INTERFACE_H_ */
