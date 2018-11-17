#include "etool/layers/list.h"
#include <functional>

#include "catch.hpp"

using namespace etool;

TEST_CASE("Layers", "[root]")
{
    using message_type = std::string;
    using layer_type = layers::pass_through<message_type, message_type,
                                            layers::traits::raw_pointers,
                                            layers::traits::unique_pointer>;

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
        execute_layer(std::function<void(message_type)> call)
            : call_(std::move(call))
        {
        }

    private:
        void from_upper(message_type msg) override
        {
            call_(std::move(msg));
        }
        void from_lower(message_type msg) override
        {
            call_(std::move(msg));
        }
        std::function<void(message_type)> call_;
    };

    SECTION("layer list")
    {
        extra_symbol root('!');
        std::string result;
        auto show = [&](message_type msg) { result.swap(msg); };
        root.set_lower(std::make_unique<extra_symbol>('1'))
            ->set_lower(std::make_unique<extra_symbol>('2'))
            ->set_lower(std::make_unique<extra_symbol>('3'))
            ->set_lower(std::make_unique<extra_symbol>('4'))
            ->set_lower(std::make_unique<execute_layer>(show));
        root.from_upper("");
        execute_layer show_layer(show);
        root.set_upper(&show_layer);
        REQUIRE(result == "!1234");
    }
}
