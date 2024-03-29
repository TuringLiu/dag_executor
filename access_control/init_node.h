
#include "dag.h"
#include "gen-cpp/data_define_template_types.h"

class InitNode: public Node
{
public:
    InitNode(const std::string& node_name) : Node(node_name) {} 
    virtual void access_control(Context& ctx) override {
        if(circuit_flag_ == true) copy_from_ctx(ctx);
        run_(ctx.comm.user_data);
        if(circuit_flag_ == true && status_ == true) copy_on_success(ctx);
    }
private:
    void run_(const UserData& CTX_COMM_user_data) ;

    virtual void copy_from_ctx(Context& ctx) override{
      CTX_eng = ctx.eng;

    }
    virtual void copy_on_success(Context& ctx) override{
    ctx.eng = CTX_eng;
   
    }
private:
    std::atomic<bool> circuit_flag_{false};
    std::atomic<bool> status_{false};

  Engineering CTX_eng;

};        
