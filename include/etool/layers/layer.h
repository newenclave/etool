#ifndef ETOOL_LAYERS_LAYER_H
#define ETOOL_LAYERS_LAYER_H

#include "etool/layers/traits/raw_pointers.h"
#include "etool/layers/traits/unique_pointers.h"

namespace etool { namespace layers {
    template <typename ReqType, typename ResType = ReqType,
              typename UpperPtrTrait = traits::raw_pointers,
              typename LowerPtrTrait = UpperPtrTrait>
    class layer {
        using upper_traits = UpperPtrTrait;
        using lower_traits = LowerPtrTrait;
        using this_type = layer<ReqType, ResType, upper_traits, lower_traits>;

    public:
        using req_type = ReqType;
        using res_type = ResType;
        using upper_pointer_type =
            typename upper_traits::template pointer_type<this_type>;
        using lower_pointer_type =
            typename lower_traits::template pointer_type<this_type>;

        virtual ~layer()
        {
            upper_traits::destroy(upper_);
            lower_traits::destroy(lower_);
        }

        layer() = default;

        layer(this_type&& other)
        {
            swap(other);
        }

        layer& operator=(this_type&& other)
        {
            swap(other);
            return *this;
        }

        virtual upper_pointer_type& set_upper(upper_pointer_type upper)
        {
            std::swap(upper_, upper);
            return get_upper();
        }

        virtual lower_pointer_type& set_lower(lower_pointer_type lower)
        {
            std::swap(lower_, lower);
            return get_lower();
        }

    public:
        virtual void from_upper(res_type msg) = 0; // from upper layer

        virtual void from_lower(req_type msg) = 0; // from lower layer

        upper_pointer_type& get_upper()
        {
            return upper_;
        }

        lower_pointer_type& get_lower()
        {
            return lower_;
        }

        const upper_pointer_type& get_upper() const
        {
            return upper_;
        }

        const lower_pointer_type& get_lower() const
        {
            return lower_;
        }

    protected:
        void swap(this_type& other)
        {
            upper_traits::swap(upper_, other.upper_);
            lower_traits::swap(lower_, other.lower_);
        }

        void send_to_lower(res_type msg)
        {
            lower_traits::get_write(lower_)->from_upper(std::move(msg));
        }

        void send_to_upper(req_type msg)
        {
            upper_traits::get_write(upper_)->from_lower(std::move(msg));
        }

        bool has_upper() const
        {
            return !upper_traits::is_empty(upper_);
        }

        bool has_lower() const
        {
            return !lower_traits::is_empty(lower_);
        }

    private:
        upper_pointer_type upper_ = nullptr;
        lower_pointer_type lower_ = nullptr;
    };

    template <typename ReqType, typename ResType = ReqType,
              typename UpperPtrTrait = traits::raw_pointers,
              typename LowerPtrTrait = UpperPtrTrait>
    class pass_through
        : public layer<ReqType, ResType, UpperPtrTrait, LowerPtrTrait> {

        using upper_traits = UpperPtrTrait;
        using lower_traits = LowerPtrTrait;
        using this_type
            = pass_through<ReqType, ResType, upper_traits, lower_traits>;
        using super_type = layer<ReqType, ResType, upper_traits, lower_traits>;

    public:
        using req_type = typename super_type::req_type;
        using res_type = typename super_type::res_type;
        using upper_pointer_type = typename super_type::upper_pointer_type;
        using lower_pointer_type = typename super_type::lower_pointer_type;

    public:
        void from_upper(res_type msg) override // from upper layer
        {
            this->send_to_lower(std::move(msg));
        }

        void from_lower(req_type msg) override // from lower layer
        {
            this->send_to_upper(std::move(msg));
        }
    };

}}

#endif
