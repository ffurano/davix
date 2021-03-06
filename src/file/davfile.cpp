/*
 * This File is part of Davix, The IO library for HTTP based protocols
 * Copyright (C) 2013  Adrien Devresse <adrien.devresse@cern.ch>, CERN
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/





#include <davix_internal.hpp>
#include <file/davfile.hpp>
#include <fileops/chain_factory.hpp>

namespace Davix{


DavFile::Iterator createIterator(DavFile::DavFileInternal& f, const RequestParams * params);


struct DavFile::DavFileInternal{

    DavFileInternal(Context & c, const Uri & u, const RequestParams & params = RequestParams()) :
        _c(c), _u(u), _params(params) {}

    DavFileInternal(const DavFileInternal & orig) : _c(orig._c), _u(orig._u), _params(orig._params) {}

    Context & _c;
    Uri _u;
    RequestParams _params;

    HttpIOChain & getIOChain(HttpIOChain & c){
        return ChainFactory::instanceChain(CreationFlags(), c);
    }

    IOChainContext getIOContext(const RequestParams * params){
        return IOChainContext(_c, _u, (params)?(params):(&_params));
    }

    DavFile::Iterator createIterator(const RequestParams * params);


    static void check_iterator(DavFile::Iterator::Internal* ptr){
        if(ptr == NULL)
            throw DavixException(davix_scope_directory_listing_str(), StatusCode::InvalidArgument, "Usage of an invalid Iterator");
    }


};


struct DavFile::Iterator::Internal{

    Internal(DavFile::DavFileInternal & f, const RequestParams* p) :
        io_chain(),
        io_context(f.getIOContext(p))
    {
        f.getIOChain(io_chain);
        io_chain.nextSubItem(io_context, name, info);
    }

    HttpIOChain io_chain;
    IOChainContext io_context;
    std::string name;
    StatInfo info;
};




DavFile::Iterator DavFile::DavFileInternal::createIterator(const RequestParams * params){
    DavFile::Iterator it;
    it.d_ptr.reset(new DavFile::Iterator::Internal(*this, params));
    return it;
}


bool DavFile::Iterator::next(){
    return d_ptr->io_chain.nextSubItem(d_ptr->io_context, d_ptr->name, d_ptr->info);
}

const std::string & DavFile::Iterator::name(){
    DavFileInternal::check_iterator(d_ptr.get());
    return d_ptr->name;
}

const StatInfo & DavFile::Iterator::info(){
    DavFileInternal::check_iterator(d_ptr.get());
    return d_ptr->info;
}


DavFile::DavFile(Context &c, const Uri &u) :
    d_ptr(new DavFileInternal(c,u))
{

}

DavFile::DavFile(Context &c, const RequestParams & params, const Uri &u) :
    d_ptr(new DavFileInternal(c,u,params))
{

}



DavFile::DavFile(const DavFile & orig): d_ptr(new DavFileInternal(*orig.d_ptr)){

}

DavFile::~DavFile(){
    delete d_ptr;
}


const Uri &  DavFile::getUri() const{
    return d_ptr->_u;
}


std::vector<DavFile> DavFile::getReplicas(const RequestParams *_params, DavixError **err) throw(){
    std::vector<DavFile> res;
    TRY_DAVIX{
        HttpIOChain chain;
        IOChainContext io_context = d_ptr->getIOContext(_params);
        return d_ptr->getIOChain(chain).getReplicas(io_context, res);
    }CATCH_DAVIX(err)
    return res;
}


dav_ssize_t DavFile::getAllReplicas(const RequestParams* params, ReplicaVec & v, DavixError **err){
    (void) params;
    (void) v;
    Davix::DavixError::setupError(err, davix_scope_http_request(), StatusCode::OperationNonSupported, " GetAllReplicas Function not supported, please use GetReplicas()");
    return -1;
}

dav_ssize_t DavFile::readPartialBufferVec(const RequestParams *params, const DavIOVecInput * input_vec,
                      DavIOVecOuput * output_vec,
                      const dav_size_t count_vec, DavixError** err) throw(){
    TRY_DAVIX{
        HttpIOChain chain;
        IOChainContext io_context = d_ptr->getIOContext(params);
        return d_ptr->getIOChain(chain).preadVec(io_context, input_vec, output_vec, count_vec);
    }CATCH_DAVIX(err)
    return -1;
}


dav_ssize_t DavFile::readPartial(const RequestParams *params, void* buff, dav_size_t count, dav_off_t offset, DavixError** err) throw(){
    TRY_DAVIX{
        HttpIOChain chain;
        IOChainContext io_context = d_ptr->getIOContext(params);
        return d_ptr->getIOChain(chain).pread(io_context, buff, count, offset);
    }CATCH_DAVIX(err)
    return -1;
}

int DavFile::deletion(const RequestParams *params, DavixError **err) throw(){
    TRY_DAVIX{
        deletion(params);
        return 0;
    }CATCH_DAVIX(err)
    return -1;
}

void DavFile::deletion(const RequestParams *params){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).deleteResource(io_context);
}

dav_ssize_t DavFile::getToFd(const RequestParams* params,
                        int fd,
                        DavixError** err) throw(){
    TRY_DAVIX{
        return DavFile::getToFd(params, fd, 0, err);
    }CATCH_DAVIX(err)
    return -1;
}

dav_ssize_t DavFile::getToFd(const RequestParams* params,
                        int fd,
                        dav_size_t size_read,
                        DavixError** err) throw(){
    TRY_DAVIX{
        HttpIOChain chain;
        IOChainContext io_context = d_ptr->getIOContext(params);
        return d_ptr->getIOChain(chain).readToFd(io_context, fd, size_read);
    }CATCH_DAVIX(err)
    return -1;
}



dav_ssize_t DavFile::getFull(const RequestParams* params,
                        std::vector<char> & buffer,
                        DavixError** err) throw(){
    TRY_DAVIX{
        return get(params, buffer);
    }CATCH_DAVIX(err)
    return -1;
}


dav_ssize_t DavFile::get(const RequestParams* params,
                        std::vector<char> & buffer){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    return d_ptr->getIOChain(chain).readFull(io_context, buffer);
}


int DavFile::putFromFd(const RequestParams* params,
              int fd,
              dav_size_t size,
              DavixError** err) throw(){
    TRY_DAVIX{
        put(params, fd, size);
        return 0;
    }CATCH_DAVIX(err)
    return -1;
}

void DavFile::put(const RequestParams *params, int fd, dav_size_t size_write){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).writeFromFd(io_context, fd, size_write);
}

void DavFile::put(const RequestParams *params, const DataProviderFun &callback, dav_size_t size_write){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).writeFromCb(io_context, callback, size_write);
}


static dav_ssize_t buffer_mapper(void* buffer, dav_size_t max_size, const char* origin_buffer, dav_size_t buffer_size, dav_size_t* written_bytes){
    dav_ssize_t res;

    if(max_size ==0 || buffer_size == *written_bytes){
        *written_bytes =0;
        return 0;
    }

    res = std::min(max_size, buffer_size - *written_bytes);
    memcpy(buffer, origin_buffer+ *written_bytes, res);
    *written_bytes += res;

    return res;
}

void DavFile::put(const RequestParams *params, const char *buffer, dav_size_t size_write){
    using namespace boost;
    dav_size_t written_bytes=0;
    put(params, bind(buffer_mapper, _1, _2, buffer, size_write, &written_bytes), size_write);

}



void DavFile::move(const RequestParams *params, DavFile & destination){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).move(io_context, destination.getUri().getString());
}



int DavFile::makeCollection(const RequestParams *params, DavixError **err) throw(){
    TRY_DAVIX{
        makeCollection(params);
        return 0;
    }CATCH_DAVIX(err)
    return -1;
}


void DavFile::makeCollection(const RequestParams *params){
    RequestParams _params(params);
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).makeCollection(io_context);
}

int DavFile::stat(const RequestParams* params, struct stat * st, DavixError** err) throw(){
    TRY_DAVIX{
        if(st == NULL)
            throw DavixException(davix_scope_meta(), StatusCode::InvalidArgument, "Argument stat NULL");

        StatInfo info;
        statInfo(params, info).toPosixStat(*st);
        return 0;
    }CATCH_DAVIX(err)
    return -1;
}

StatInfo& DavFile::statInfo(const RequestParams *params, StatInfo &info){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(params);
    d_ptr->getIOChain(chain).statInfo(io_context, info);
    return info;
}

DavFile::Iterator DavFile::listCollection(const RequestParams *params){
    return d_ptr->createIterator(params);
}

int DavFile::checksum(const RequestParams *params, std::string & checksm, const std::string & chk_algo, DavixError **err) throw(){
    TRY_DAVIX{
        HttpIOChain chain;
        IOChainContext io_context = d_ptr->getIOContext(params);
        d_ptr->getIOChain(chain).checksum(io_context, checksm, chk_algo);
        return 0;
    }CATCH_DAVIX(err)
    return -1;
}

void DavFile::prefetchInfo(off_t offset, dav_size_t size_read, advise_t adv){
    HttpIOChain chain;
    IOChainContext io_context = d_ptr->getIOContext(NULL);
    d_ptr->getIOChain(chain).prefetchInfo(io_context, offset, size_read, adv);
}



} //Davix



std::ostream & operator<<(std::ostream & out,  Davix::DavFile & file){
        std::vector<char> buffer;

        file.get(NULL, buffer);
        out.write(&buffer[0], buffer.size());
        return out;
}

std::istream & operator>>(std::istream & in, Davix::DavFile & file){
    std::vector<char> buffer((std::istream_iterator<char>(in)), std::istream_iterator<char>());

    file.put(NULL, &(buffer.at(0)), buffer.size());
    return in;
}
