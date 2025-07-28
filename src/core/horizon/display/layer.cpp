#include "core/horizon/display/layer.hpp"

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/os.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"
#include "core/hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace hydra::horizon::display {

void Layer::AcquirePresentTexture(
    std::vector<std::chrono::nanoseconds>& out_dt_list) {
    // Get the buffer to present
    auto& binder = OS_INSTANCE.GetDisplayDriver().GetBinder(binder_id);

    i32 slot = binder.ConsumeBuffer(input, out_dt_list);
    if (slot == -1)
        return;
    const auto& buffer = binder.GetBuffer(slot);

    present_texture = GPU_INSTANCE.GetTexture(
        (*KERNEL_INSTANCE.GetProcessManager().Begin())->GetMmu(),
        buffer.nv_buffer); // HACK
}

bool Layer::Present(u32 width, u32 height) {
    if (!present_texture)
        return false;

    // Src rect
    IntRect2D src_rect;
    src_rect.origin.x() = input.rect.left;
    src_rect.origin.y() =
        input.rect.top; // Convert from top left to bottom left origin
    src_rect.size.x() = input.rect.right - input.rect.left;
    src_rect.size.y() = input.rect.bottom - input.rect.top;

    // HACK
    if (src_rect.size.x() == 0) {
        src_rect.size.x() = present_texture->GetDescriptor().width;
        ONCE(LOG_WARN(Other, "Invalid src width"));
    }
    if (src_rect.size.y() == 0) {
        src_rect.size.y() = present_texture->GetDescriptor().height;
        ONCE(LOG_WARN(Other, "Invalid src height"));
    }

    if (any(input.transform_flags & horizon::display::TransformFlags::FlipH)) {
        src_rect.origin.x() += src_rect.size.x();
        src_rect.size.x() = -src_rect.size.x();
    }
    if (any(input.transform_flags & horizon::display::TransformFlags::FlipV)) {
        src_rect.origin.y() += src_rect.size.y();
        src_rect.size.y() = -src_rect.size.y();
    }
    if (any(input.transform_flags & horizon::display::TransformFlags::Rot90)) {
        // TODO: how does this work? Is the aspect ratio kept intact?
        ONCE(LOG_NOT_IMPLEMENTED(Other, "Rotating by 90 degrees"));
    }

    // Dst rect
    const auto src_width = abs(src_rect.size.x());
    const auto src_height = abs(src_rect.size.y());

    IntRect2D dst_rect;
    auto scale_x = (f32)width / (f32)src_width;
    auto scale_y = (f32)height / (f32)src_height;
    if (scale_x > scale_y) {
        const auto dst_width = static_cast<i32>(src_width * scale_y);
        dst_rect.origin = int2({static_cast<i32>(width - dst_width) / 2, 0});
        dst_rect.size = int2({dst_width, static_cast<i32>(height)});
    } else {
        const auto dst_height = static_cast<i32>(src_height * scale_x);
        dst_rect.origin = int2({0, static_cast<i32>(height - dst_height) / 2});
        dst_rect.size = int2({static_cast<i32>(width), dst_height});
    }

    // Draw
    RENDERER_INSTANCE.DrawTextureToSurface(present_texture, src_rect, dst_rect);

    present_texture = nullptr;

    return true;
}

} // namespace hydra::horizon::display
