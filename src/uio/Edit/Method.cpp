//  Copyright (c) 2018 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#include <uio/Edit/Method.h>

#include <obj/Reflect/Method.h>
#include <obj/Reflect/Meta.h>

#include <uio/Edit/Value.h>
#include <ui/Structs/Container.h>

#include <string>

namespace mud
{
	struct CallableEditState : public NodeState
	{
		CallableEditState(Callable& callable) : m_call(callable) {}
		CallableEditState(Callable& callable, Ref object) : m_call(callable, object) {}
		Call m_call;
	};

	using string = std::string;

	bool call_edit(Widget& parent, Call& call)
	{
		bool method = strcmp(call.m_callable->m_params[0].m_name, "self") == 0;
		uint16_t offset = method ? 1 : 0;
		for(uint16_t i = offset; i < call.m_callable->m_params.size(); ++i)
		{
			const Param& param = call.m_callable->m_params[i];
			Ref value = call.m_arguments[i];
			field_edit(parent, param.m_name, value, param.nullable());
		}

		/*uint16_t offset = call.m_arguments.size() - call.m_callable->m_params.size();
		for(Param& param : call.m_callable->m_params)
			field_edit(parent, param.m_name, call.m_arguments[param.m_index + offset], param.nullable());*/
		return false;
	}

	void callable_edit(Widget& parent, Callable& callable)
	{
		Widget& self = ui::row(parent);
		CallableEditState& state = self.state<CallableEditState>(callable);
		call_edit(self, state.m_call);
	}

	void function_edit(Widget& parent, Function& function)
	{
		callable_edit(parent, function);
	}

	void method_edit(Widget& parent, Ref object, Method& method)
	{
		Widget& self = ui::row(parent);
		CallableEditState& state = self.state<CallableEditState>(method, object);
		if(ui::button(self, method.m_name).activated())
			state.m_call();
		call_edit(self, state.m_call);
	}

	void method_edit_modal(Widget& parent, Ref object, Method& method)
	{
		string name = "Call method " + string(method.m_object_type->m_name) + "::" + method.m_name;
		if(modal_dialog(parent, name.c_str()))
			method_edit(parent, object, method);
	}

	void method_trigger(Widget& parent, Ref object, Method& method)
	{
		if(ui::modal_button(parent, parent, method.m_name, uint32_t(1 << 0)))
			method_edit_modal(parent, object, method);
	}

	void method_hook(Widget& parent, Ref object, Method& method)
	{
		method_edit(parent, object, method);
	}
}
