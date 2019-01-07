#include "rangememorystorage.h"

namespace BitTorrent {

    RangeMemoryStorage::RangeMemoryStorage(const file_storage& fs)
        : libtorrent::storage_interface(fs)
    {

    }

    int RangeMemoryStorage::writev(span<iovec_t const> bufs
                                   , piece_index_t piece
                                   , int offset
                                   , open_mode_t flags
                                   , storage_error& ec) {
        Q_UNUSED(bufs);
        Q_UNUSED(piece);
        Q_UNUSED(offset);
        Q_UNUSED(flags);
        Q_UNUSED(ec);

        for(span<iovec_t const>::iterator itr = bufs.begin(); itr != bufs.end(); ++itr) {
            auto size = itr->size();
            auto p = itr->data();
        }
        return -1;
    }
}
