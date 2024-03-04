
#include "dag.h"

class ReturnNode: public Node
{
public:
    ReturnNode(const std::string& node_name) : Node(node_name) {} 
    virtual void access_control(Context& ctx) override {
        if(circuit_flag_ == true) copy_from_ctx(ctx);
        run_(ctx.video.list, ctx.comm.user_data);
        if(circuit_flag_ == true && status_ == true) copy_on_success(ctx);
    }
private:
    void run_(std::vector<long long> CTX_VIDEO_list, const UserData& CTX_COMM_user_data) ;

    virtual void copy_from_ctx(Context& ctx) override{

    }
    virtual void copy_on_success(Context& ctx) override{

    }
private:
    std::atomic<bool> circuit_flag_{false};
    std::atomic<bool> status_{false};


};        
