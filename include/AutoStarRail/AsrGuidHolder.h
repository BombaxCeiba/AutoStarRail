#ifndef ASR_GUIDHOLDER_H
#define ASR_GUIDHOLDER_H

template <class T>
struct AsrIidHolder;

template <class T>
[[nodiscard]]
auto AsrIidOf() -> const struct _asr_GUID&;

#define ASR_DEFINE_GUID_HOLDER(                                                \
    type,                                                                      \
    l,                                                                         \
    w1,                                                                        \
    w2,                                                                        \
    b1,                                                                        \
    b2,                                                                        \
    b3,                                                                        \
    b4,                                                                        \
    b5,                                                                        \
    b6,                                                                        \
    b7,                                                                        \
    b8)                                                                        \
    extern "C++"                                                               \
    {                                                                          \
        template <>                                                            \
        struct AsrIidHolder<struct type>                                       \
        {                                                                      \
            static constexpr AsrGuid iid =                                     \
                {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};                 \
        };                                                                     \
        template <>                                                            \
        constexpr const AsrGuid& AsrIidOf<struct type>()                       \
        {                                                                      \
            return AsrIidHolder<type>::iid;                                    \
        }                                                                      \
        template <>                                                            \
        constexpr const AsrGuid& AsrIidOf<struct type*>()                      \
        {                                                                      \
            return AsrIidHolder<type>::iid;                                    \
        }                                                                      \
    }

#define ASR_DEFINE_CLASS_GUID_HOLDER(                                          \
    type,                                                                      \
    l,                                                                         \
    w1,                                                                        \
    w2,                                                                        \
    b1,                                                                        \
    b2,                                                                        \
    b3,                                                                        \
    b4,                                                                        \
    b5,                                                                        \
    b6,                                                                        \
    b7,                                                                        \
    b8)                                                                        \
    extern "C++"                                                               \
    {                                                                          \
        template <>                                                            \
        struct AsrIidHolder<class type>                                        \
        {                                                                      \
            static constexpr AsrGuid iid =                                     \
                {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};                 \
        };                                                                     \
        template <>                                                            \
        constexpr const AsrGuid& AsrIidOf<class type>()                        \
        {                                                                      \
            return AsrIidHolder<type>::iid;                                    \
        }                                                                      \
        template <>                                                            \
        constexpr const AsrGuid& AsrIidOf<class type*>()                       \
        {                                                                      \
            return AsrIidHolder<type>::iid;                                    \
        }                                                                      \
    }

#define ASR_UUID_OF(type) AsrIidOf<decltype(type)>()

#endif // ASR_GUIDHOLDER_H