#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::applets::album {

enum class Arg : u8 {
    ShowAlbumFiles = 0,
    ShowAllAlbumFiles = 1,
    ShowAllAlbumFilesForHomeMenu = 2,
};

} // namespace hydra::horizon::applets::album
