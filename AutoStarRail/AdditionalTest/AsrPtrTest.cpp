#include <atomic>
#include <string_view>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/IAsrBase.h>
#include <gtest/gtest.h>

namespace
{
    class TestImpl : public IAsrBase
    {
    private:
        std::atomic_int64_t count_{0};

    public:
        TestImpl()
        {
        }
        std::int64_t AddRef() override
        {
            count_ += 1;
            return count_;
        }
        std::int64_t Release() override
        {
            count_ -= 1;
            if (count_ == 0)
            {
                delete this;
                return 0;
            }
            return count_;
        }
        std::int64_t GetRefCount() const noexcept
        {
            return count_;
        }
        ASR_METHOD QueryInterface(const AsrGuid& id, void** pp_object) override
        {
            *pp_object = nullptr;
            return ASR_E_NO_IMPLEMENTATION;
        }
    };

    AsrResult MakeTestImpl(void** out_ptr)
    {
        auto result = new TestImpl();
        static_cast<IAsrBase*>(result)->AddRef();
        *out_ptr = result;
        return ASR_S_OK;
    }

    TEST(ASRPtrTest, BasicTest)
    {
        TestImpl* p_test_impl = nullptr;
        {
            ASR::AsrPtr<TestImpl> impl;

            AsrResult result = MakeTestImpl(impl.PutVoid());
            p_test_impl = impl.Get();
            EXPECT_EQ(impl->GetRefCount(), 1);
            impl->AddRef();
            EXPECT_EQ(impl->GetRefCount(), 2);
        }
        EXPECT_EQ(p_test_impl->GetRefCount(), 1);
        p_test_impl->Release();
    }
}