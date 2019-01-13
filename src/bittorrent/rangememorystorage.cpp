#include "rangememorystorage.h"
#include "session.h"

namespace BitTorrent {

    int bufs_size(span<iovec_t const> bufs)
    {
        std::ptrdiff_t size = 0;
        for (auto buf : bufs) size += buf.size();
        return int(size);
    }

    RangeMemoryStorage::RangeMemoryStorage(const file_storage& fs, Session* s, int pieceLen)
        : libtorrent::storage_interface(fs), session(s), pieceLength(pieceLen)
    {}

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
        //qDebug() << Q_FUNC_INFO << "(piece: " << piece << ", offset:" << offset << ")";
        Q_UNUSED(bufs);
        Q_UNUSED(piece);
        Q_UNUSED(offset);
        Q_UNUSED(flags);
        Q_UNUSED(ec);

        int totalSize = bufs_size(bufs);
        pieceStatus.insert(static_cast<int>(piece), pieceStatus.value(static_cast<int>(piece), 0) + totalSize);
        //qDebug() << "bufs size " << totalSize;

        for(span<iovec_t const>::iterator itr = bufs.begin(); itr != bufs.end(); ++itr) {            
            auto size = itr->size();
            auto p = itr->data();
            //qDebug() << "buffer size " << size;
            Q_UNUSED(p);
            Q_UNUSED(size);
        }

        if (totalSize == pieceLength) {
            qDebug() << "piece completed " << piece;
            int pieceIndex = 0;
            for(auto prio: session->handle.get_piece_priorities()) {
                if (prio == libtorrent::dont_download && pieceIndex < 10) {
                    qDebug() << "set top priority to " << pieceIndex;
                    session->handle.piece_priority(pieceIndex, libtorrent::top_priority);
                }

                ++pieceIndex;
            }
        }

        return totalSize;
    }
}
