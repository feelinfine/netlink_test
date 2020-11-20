#include <string>
#include <fstream>

#include "user_nl.h"

#define LOG(s) if (s.is_open()) s

static inline void assert_ok(NL_RV _rv, const char* _fn)
{
	if (_rv)						\
		throw std::runtime_error(std::string("Function ") + _fn + " failed:" + nl_rv_str(_rv) + " Errno: " + strerror(errno));
}

struct NlGuard
{
	NlGuard()
	{
		static const __u32 TIMEOUT_IN_SEC = 5;

		auto rv = nl_init(&m_ctx, TIMEOUT_IN_SEC);

		if (rv)
		{
			nl_free(&m_ctx);
			throw std::runtime_error(
				std::string("Initialization failed") + nl_rv_str(rv)
			);
		}
	}

	NlGuard(const NlGuard&) = delete;
	NlGuard& operator=(const NlGuard&) = delete;

	auto* get()
	{
		return &m_ctx;
	}

	~NlGuard()
	{
		nl_free(&m_ctx);
	}

private:
	NetlinkCtx m_ctx;
};

int main(int _ac, char** _av)
{
	std::ofstream file;
	
	try 
	{
		file.open(STRING(BUILD_DIR)"user_log.txt");

		if (_ac < 2)
			throw std::runtime_error("No options");

		if (strcmp(_av[1], "Wake up!") == 0)
		{

			//Send Hello message to kernel
			std::string msg = "Hello kernel, you heartless bitch!";

			NlGuard ctx;

			auto rv = nl_send(ctx.get(), TO_KERNEL, (__u8*)msg.data(), msg.size());
			assert_ok(rv, "nl_send");

			//Get 'Goodbye' message from kernel
			std::string response(128, '\0');
			__u32 size = 128;

			rv = nl_recv(ctx.get(), (__u8*)response.data(), &size);
			assert_ok(rv, "nl_recv");

			response.resize(size);

			LOG(file) << "Message from KERNEL (pid " << ctx.get()->dst.nl_pid << "): " << response;

			//Send 'Goodbye' message to kernel
			if (response == std::string("Goodbye!"))
			{
				msg = "Goodbye kernel!";
		
				rv = nl_send(ctx.get(), TO_KERNEL, (__u8*)msg.data(), msg.size());
				assert_ok(rv, "nl_send");
			}
			else
			{
				throw std::runtime_error("Invalid kernel message =/");
			}
		}
		else
		{
			throw std::runtime_error("Unknown options");
		}
	}
	catch (const std::runtime_error& _e)
	{
		LOG(file) << _e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		LOG(file) << "Unknown error" << std::endl;
		return 1;
	}

	return 0;
}