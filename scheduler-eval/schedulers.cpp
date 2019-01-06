#include "ntl/scheduler.hpp"
#include "ntl/link.hpp"

#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>

typedef ntl::gateway_registers<int> gateway_registers;

struct scheduler_cmd {
    int port;
    uint32_t quantum;
};

template <unsigned log_size>
class sched_wrapper
{
public:
    typedef ntl::scheduler<log_size> scheduler_t;
    typedef typename scheduler_t::index_t index_t;

    static void decode_gateway_address(int address, int& flow_id, int& cmd,
		    bool read)
    {
        flow_id = address >> 2;
        cmd = address & 1;
	read = address & 2;
    }

    void step(gateway_registers& g)
    {
#pragma HLS pipeline II=3
        /* Inline the gateway here */
#pragma HLS inline region
        gateway.gateway(g, [&](int addr, int& data) -> bool {
#pragma HLS inline
            int flow_id, cmd;
	    bool read;
            decode_gateway_address(addr, flow_id, cmd, read);
            return sched.rpc(cmd, &data, flow_id, read);
        });

        if (sched.update())
            return;

        index_t req;
        if (!tx_requests.empty()) {
            req = tx_requests.read();
            sched.schedule(req);
        }

        if (scheduler_decision.full())
            return;

        index_t selected_stream;
        uint32_t quota;
        if (!sched.next_flow(&selected_stream, &quota))
            return;

        scheduler_decision.write(scheduler_cmd{selected_stream, quota});
    }
    ntl::stream<index_t> tx_requests;
    ntl::stream<scheduler_cmd> scheduler_decision;
private:
    scheduler_t sched;
    ntl::gateway_impl<int> gateway;
};

#define NUM_SCHEDULERS 8
void sched_top(BOOST_PP_ENUM_PARAMS(NUM_SCHEDULERS, gateway_registers& g),
        BOOST_PP_ENUM_PARAMS(NUM_SCHEDULERS, ntl::stream<int>& tx_requests),
        BOOST_PP_ENUM_PARAMS(NUM_SCHEDULERS, ntl::stream<scheduler_cmd>& scheduler_decision))
{
#pragma HLS dataflow
#pragma HLS interface ap_ctrl_none port=return

#define BOOST_PP_LOCAL_MACRO(n) \
    GATEWAY_OFFSET(g ## n, 0x100, 0x118, 0xfc) \
    static sched_wrapper<BOOST_PP_INC(n)> sched ## n; \
    sched ## n.step(g ## n); \
    ntl::link(tx_requests ## n, sched ## n.tx_requests); \
    ntl::link(sched ## n.scheduler_decision, scheduler_decision ## n);
#define BOOST_PP_LOCAL_LIMITS (0, NUM_SCHEDULERS - 1)
%:include BOOST_PP_LOCAL_ITERATE()
}

int main()
{
    return 0;
}
