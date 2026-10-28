// OpenSSL заглушка (M0) — для будущего TLS-апгрейда auth/world каналов.
#pragma once
namespace openssl_stub {
	inline constexpr bool IsStub = true;
	// API-совместимые обёртки появятся при переходе на реальный libssl:
	// SSL_CTX_new / SSL_connect / SSL_write поверх FMMONetworkClient::Socket.
}
