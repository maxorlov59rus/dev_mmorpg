// Boost.Asio заглушка (M0). Реальный boost подкладывается в этот include-путь
// без изменения кода: #include "boost_stub/asio_compat.h" -> using namespace boost::asio.
#pragma once
namespace boost_stub {
	// Маркер компиляции заглушки; реальный буст заменит файл целиком.
	inline constexpr bool IsStub = true;
}
