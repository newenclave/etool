#include "etool/layers/list.h"
#include <functional>

#include "catch.hpp"

using namespace etool;

TEST_CASE("Layers", "[root]")
{
    using message_type = std::string;
    using layer_type = layers::pass_through<message_type>;

    class extra_symbol : public layer_type {
    public:
        extra_symbol(char c)
            : c_(c)
        {
        }
        void from_upper(message_type msg) override
        {
            if (has_lower()) {
                send_to_lower(msg + c_);
            }
        }
        void from_lower(message_type msg)
        {
            if (has_upper()) {
                msg.pop_back();
                send_to_upper(std::move(msg));
            }
        }

    private:
        char c_;
    };

    class execute_layer : public layer_type {
    public:
        execute_layer(std::function<void(const message_type&)> call)
            : call_(std::move(call))
        {
        }

    private:
        void from_upper(message_type msg) override
        {
            call_(msg);
            if (has_lower()) {
                send_to_lower(std::move(msg));
            }
        }
        void from_lower(message_type msg) override
        {
            call_(msg);
            if (has_upper()) {
                send_to_upper(std::move(msg));
            }
        }
        std::function<void(const message_type&)> call_;
    };

    SECTION("layer list")
    {
        layers::list<message_type> lst;
        std::string result;
        lst.create_back<extra_symbol>('1');
        lst.create_back<extra_symbol>('2');
        lst.create_back<extra_symbol>('3');
        lst.create_back<extra_symbol>('4');
        lst.create_back<extra_symbol>('5');

        lst.create_back<execute_layer>(
            [&](const message_type& msg) { result = msg; });
        lst.create_front<execute_layer>(
            [&](const message_type& msg) { result = msg; });
        lst.from_upper("");
        REQUIRE(result == "12345");
        lst.from_lower(result);
        REQUIRE(result == "");
    }
}
