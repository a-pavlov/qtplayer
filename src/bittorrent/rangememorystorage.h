#ifndef RANGEMEMORYSTORAGE_H
#define RANGEMEMORYSTORAGE_H

#include <QObject>
#include <QDebug>
#include <libtorrent/storage.hpp>


namespace BitTorrent {
    using namespace libtorrent;

    class RangeMemoryStorage : public libtorrent::storage_interface
    {
    public:
        RangeMemoryStorage(file_storage const& fs);

        void initialize(storage_error& se) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(se);
        }

        int readv(span<iovec_t const> bufs
                  , piece_index_t piece, int offset, open_mode_t flags, storage_error& ec) override;

        int writev(span<iovec_t const> bufs
            , piece_index_t piece, int offset, open_mode_t flags, storage_error& ec) override;
        // This function is called when first checking (or re-checking) the
        // storage for a torrent. It should return true if any of the files that
        // is used in this storage exists on disk. If so, the storage will be
        // checked for existing pieces before starting the download.
        //
        // If an error occurs, ``storage_error`` should be set to reflect it.
        bool has_any_file(storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(ec);
            return false;
        }

        // change the priorities of files. This is a fenced job and is
        // guaranteed to be the only running function on this storage
        // when called
        void set_file_priority(aux::vector<download_priority_t, file_index_t>& prio
                               , storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(prio);
            Q_UNUSED(ec);
        }

        // This function should move all the files belonging to the storage to
        // the new save_path. The default storage moves the single file or the
        // directory of the torrent.
        //
        // Before moving the files, any open file handles may have to be closed,
        // like ``release_files()``.
        //
        //If an error occurs, ``storage_error`` should be set to reflect it.
        virtual status_t move_storage(std::string const& save_path
                                      , move_flags_t flags, storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(save_path);
            Q_UNUSED(flags);
            Q_UNUSED(ec);
            return status_t::no_error;
        }

        // This function should verify the resume data ``rd`` with the files
        // on disk. If the resume data seems to be up-to-date, return true. If
        // not, set ``error`` to a description of what mismatched and return false.
        //
        // The default storage may compare file sizes and time stamps of the files.
        //
        // If an error occurs, ``storage_error`` should be set to reflect it.
        //
        // This function should verify the resume data ``rd`` with the files
        // on disk. If the resume data seems to be up-to-date, return true. If
        // not, set ``error`` to a description of what mismatched and return false.
        //
        // If the ``links`` pointer is non-empty, it has the same number
        // of elements as there are files. Each element is either empty or contains
        // the absolute path to a file identical to the corresponding file in this
        // torrent. The storage must create hard links (or copy) those files. If
        // any file does not exist or is inaccessible, the disk job must fail.
        bool verify_resume_data(add_torrent_params const& rd
            , aux::vector<std::string, file_index_t> const& links
                                , storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(rd);
            Q_UNUSED(links);
            Q_UNUSED(ec);
            return true;
        }

        // This function should release all the file handles that it keeps open
        // to files belonging to this storage. The default implementation just
        // calls file_pool::release_files().
        //
        // If an error occurs, ``storage_error`` should be set to reflect it.
        //
        void release_files(storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(ec);
        }

        // Rename the file with index ``file`` to name ``new_name``.
        //
        // If an error occurs, ``storage_error`` should be set to reflect it.
        //
        void rename_file(file_index_t index, std::string const& new_filename
                         , storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(index);
            Q_UNUSED(new_filename);
            Q_UNUSED(ec);
        }

        // This function should delete some or all of the storage for this torrent.
        // The ``options`` parameter specifies whether to delete all files or just
        // the partfile. ``options`` are set to the same value as the options
        // passed to session::remove_torrent().
        //
        // If an error occurs, ``storage_error`` should be set to reflect it.
        //
        // The ``disk_buffer_pool`` is used to allocate and free disk buffers. It
        // has the following members:
        //
        // .. code:: c++
        //
        //		struct disk_buffer_pool
        //		{
        //			char* allocate_buffer(char const* category);
        //			void free_buffer(char* buf);
        //
        //			char* allocate_buffers(int blocks, char const* category);
        //			void free_buffers(char* buf, int blocks);
        //
        //			int block_size() const { return m_block_size; }
        //
        //		};
        virtual void delete_files(remove_flags_t options, storage_error& ec) override {
            qDebug() << Q_FUNC_INFO;
            Q_UNUSED(options);
            Q_UNUSED(ec);
        }
    };

    inline libtorrent::storage_interface* RangeMemoryStorageConstructor(const libtorrent::storage_params& sp, file_pool& fp) {
        return new RangeMemoryStorage(sp.files);
    }
}
#endif // RANGEMEMORYSTORAGE_H
