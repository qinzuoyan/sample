#include <boost/shared_ptr.hpp>
#include <glog/logging.h>

int main(int argc, char** argv)
{
    boost::shared_ptr<int> p(new int);
    LOG(INFO) << "aaa";
    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
