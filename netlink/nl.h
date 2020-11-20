#ifndef __NETLINK_H__
#define __NETLINK_H__

#ifndef BUILD_DIR
	#error "BUILD_DIR macro not defined! Should be defined in Makefile"
#endif

#if !defined USER_APP && !defined KERNEL_MOD
    #error "Macros USER_APP or KERNEL_MOD are no defined!"
#endif

#include <linux/netlink.h>

static const __u32 UNICAST_MASK = 0;

#define NETLINK_TEST_PROTO 25

//Define own types with blackjack and hookers
#define TEST_USR_MSG NLMSG_MIN_TYPE + 1
#define TEST_KRL_MSG TEST_USR_MSG + 1

#define AUTO_PID 0

typedef enum _NL_RV
{
    RV_SUCCESS = 0,
    RV_SOCKET_ERROR,
    RV_CTX_ERROR,
    RV_SEND_ERROR,
    RV_RECV_ERROR,
    RV_PARAM_INVALID,
    RV_UNKNOWN_ERROR,
    RV_BUFFER_ERROR,
    RV_INVALID_MESSAGE
} NL_RV;

#define RET_CASE_NAME(a) case a : return #a

static const char* nl_rv_str(NL_RV _rv)
{
    switch (_rv)
    {
        RET_CASE_NAME(RV_SUCCESS);
        RET_CASE_NAME(RV_SOCKET_ERROR);
        RET_CASE_NAME(RV_CTX_ERROR);
        RET_CASE_NAME(RV_SEND_ERROR);
        RET_CASE_NAME(RV_RECV_ERROR);
        RET_CASE_NAME(RV_PARAM_INVALID);
        RET_CASE_NAME(RV_UNKNOWN_ERROR);
        RET_CASE_NAME(RV_BUFFER_ERROR);
        RET_CASE_NAME(RV_INVALID_MESSAGE);
        default:
            return "Invalid RV";
    }
}

typedef enum _DST_TYPE
{
    TO_KERNEL = 0
} DST_TYPE;

typedef struct _NetlinkCtx
{
#ifdef USER_APP
    int sock;
#else
    struct sock* sock;
#endif

    struct sockaddr_nl src;
    struct sockaddr_nl dst;
    unsigned int seq;
} NetlinkCtx;

#define ASSERT_NOT_NULL(p) if (p) if (!p) return RV_PARAM_INVALID
#define ASSERT_CTX_NOT_NULL(ctx) if (!ctx) return RV_CTX_ERROR;

#define STRING_(a) #a
#define STRING(a) STRING_(a)

#endif