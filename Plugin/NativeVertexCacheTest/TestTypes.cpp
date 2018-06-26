#include "Plugin/PrecompiledHeader.h"
#include "Plugin/Types.h"

static bool near_equals(float a, float b, float eps)
{
    return std::abs(a - b) <= std::abs(eps);
}

static void test_failed(const char *format, ...)
{
    char buf[1024];

    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    throw std::runtime_error(buf);
}


void RunTest_Types()
{
    using namespace nvc;

    // half
    {
        float test_data[] = {
            //0.00001f,
            0.0001f,
            0.001f,
            0.01f,
            0.1f,
            1.0f,
            10.0f,
            100.0f,
            1000.0f,
            10000.0f,
            //100000.0f,

            -0.0001f,
            -0.001f,
            -0.01f,
            -0.1f,
            -1.0f,
            -10.0f,
            -100.0f,
            -1000.0f,
            -10000.0f,
        };
        for (float v : test_data) {
            half h(v);
            if (!near_equals(v, h.to_float(), 0.001f)) {
                test_failed("%f : %f\n", v, h.to_float());
            }
        }
    }

    // snorm16
    {
        float test_data[] = {
            0.0001f,
            0.001f,
            0.01f,
            0.1f,
            1.0f,

            -0.0001f,
            -0.001f,
            -0.01f,
            -0.1f,
            -1.0f,
        };
        for (float v : test_data) {
            snorm16 h(v);
            if (!near_equals(v, h.to_float(), 0.001f)) {
                test_failed("%f : %f\n", v, h.to_float());
            }
        }
    }

    // unorm16
    {
        float test_data[] = {
            0.0001f,
            0.001f,
            0.01f,
            0.1f,
            1.0f,
        };
        for (float v : test_data) {
            unorm16 h(v);
            if (!near_equals(v, h.to_float(), 0.001f)) {
                test_failed("%f : %f\n", v, h.to_float());
            }
        }
    }
}
