#include <render_queue.hpp>

namespace OpenGL {
    RenderQueue RenderQueue::create(RenderState* render_state) {
        RenderQueue ret = {};
        ret.render_state = render_state;

        return ret;
    }
}