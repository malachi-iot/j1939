#pragma once

#include "menu.h"

namespace menu {
namespace layer1 {


template <class ...Actions>
template <class TStreambuf, class TBase>
void Menu<Actions...>::render(estd::detail::basic_ostream<TStreambuf, TBase>& out)
{
    //out << "layer1 menu" << estd::endl << estd::endl;

    actions::visitor::visit(RenderFunctor{}, out);
}


}
}