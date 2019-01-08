#include "rangememorystorage.h"

namespace BitTorrent {

    int bufs_size(span<iovec_t const> bufs)
    {
        std::ptrdiff_t size = 0;
        for (auto buf : bufs) size += buf.size();
        return int(size);
    }

    RangeMemoryStorage::RangeMemoryStorage(const file_storage& fs)
        : libtorrent::storage_interface(fs)
    {

    }

    int RangeMemoryStorage::readv(span<iovec_t const> bufs
              , piece_index_t piece, int offset, open_mode_t flags, storage_error& ec) {
        Q_UNUSED(bufs);
        Q_UNUSED(piece);
        Q_UNUSED(offset);
        Q_UNUSED(flags);
        Q_UNUSED(ec);
        ec.ec = error_code(boost::system::errc::operation_not_permitted, generic_category());
        return -1;
    }

    int RangeMemoryStorage::writev(span<iovec_t const> bufs
                                   , piece_index_t piece
                                   , int offset
                                   , open_mode_t flags
                                   , storage_error& ec) {
        qDebug() << Q_FUNC_INFO;
        Q_UNUSED(bufs);
        Q_UNUSED(piece);
        Q_UNUSED(offset);
        Q_UNUSED(flags);
        Q_UNUSED(ec);

        int totalSize = bufs_size(bufs);
        qDebug() << "bufs size " << totalSize;

        for(span<iovec_t const>::iterator itr = bufs.begin(); itr != bufs.end(); ++itr) {            
            auto size = itr->size();
            auto p = itr->data();
            qDebug() << "buffer size " << size;
            Q_UNUSED(p);
        }

        return totalSize;
    }
}
