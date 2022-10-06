# CS144 Lab Notes

## Lab0

### 1. 环境准备

- CS144 Lab要求Linux操作系统以及支持C++17的C++编译器
- 在Linux服务器上完成本实验，参照课程提供[文档](https://stanford.edu/class/cs144/vm_howto/vm-howto-byo.html)安装相关依赖

### 2. 抓取一个网页

- 直接访问[链接](http://cs144.keithw.org/hello)观察结果

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-05%2023.09.53.png" alt="截屏2022-10-05 23.09.53" style="zoom:50%;" />

- 运行`telnet cs144.keithw.org http`命令，并输入下列内容

  - 输入`GET /hello HTTP/1.1`以及回车键，这告诉服务器URL的路径部分
  - 输入`Host: cs144.keithw.org`以及回车键，这告诉服务器URL的主机部分
  - 输入`Connection: close`以及回车键，这告诉服务器你已经完成了HTTP请求

  结果如下图：

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-05%2023.14.26.png" alt="截屏2022-10-05 23.14.26" style="zoom:50%;" />

### 3. 使用操作系统stream socket编写网络程序

#### 3.1 获取starter code并构建

```bash
mkdir build
cd build
cmake ..
make
```

#### 3.2 CS144代码风格要求

- 使用 Resource acquisition is initialization 风格，即 RAII 风格。

- 禁止使用`malloc`和`free`函数

- 禁止使用`new`和`delete`关键字

- 禁止使用原生指针（*），若有必要，最好使用智能指针（`unique_ptr`等等）

- 避免使用模板、线程相关、各类锁机制以及虚函数

  > 这些内容在CS144中不会被用到

- 禁止使用C风格字符串(`char *`) 以及 C 风格字符串处理函数，使用`string`来代替。

- 禁止使用 C 风格强制类型转换。若有必要请使用 `static_cast`

- 传递参数给函数时，请使用常量引用类型（const Ty& t）

- 尽可能将每个变量和函数方法都声明成 const

- 禁止使用全局变量，以及尽可能让每个变量的作用域最小

#### 3.3 完成`webget`

```C++
void get_URL(const string &host, const string &path) {
    // Your code here.

    // You will need to connect to the "http" service on
    // the computer whose name is in the "host" string,
    // then request the URL path given in the "path" string.
    TCPSocket s;
    s.connect(Address(host, "http"));
    // Write the input which is simliar with the first part of Lab2
    s.write("GET " + path + " HTTP/1.1\r\n");
    s.write("Host: " + host + "\r\n");
    s.write("Connection: close \r\n");
    s.write("\r\n");
    // Then you'll need to print out everything the server sends back,
    // (not just one call to read() -- everything) until you reach
    // the "eof" (end of file).
    while(!s.eof()) {
        cout << s.read();
    }
    s.close();

    // cerr << "Function called: get_URL(" << host << ", " << path << ").\n";
    // cerr << "Warning: get_URL() has not been implemented yet.\n";
}
```

**注意**：

- HTTP 头部的每一行末尾都是以`\r\n`结尾
- 除非获取到EOF，否则必须**循环**从远程服务器读取信息，单次`read`是不够的

**结果验证**：

- 使用webget抓取网页运行结果

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-05%2023.38.34.png" alt="截屏2022-10-05 23.38.34" style="zoom:50%;" />

- 运行make check_webget的测试结果展示

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-05%2023.39.38.png" alt="截屏2022-10-05 23.39.38" style="zoom:50%;" />

### 4. 内存可靠字节流

- 实现字节流ByteStream
  - 字节流可以从写入端写入，并以相同的顺序，从读取端读取；
  - 字节流是有限的，写者可以终止写入。而读者可以在读取到字节流末尾时，产生EOF标志，不再读取；
  - 支持流量控制，以控制内存的使用；
  - 写入的字节流可能会很长，必须考虑到字节流大于缓冲区大小的情况

> byte_stream.hh

```C++
#ifndef SPONGE_LIBSPONGE_BYTE_STREAM_HH
#define SPONGE_LIBSPONGE_BYTE_STREAM_HH

#include <string>
#include <deque>

using namespace std;
//! \brief An in-order byte stream.

//! Bytes are written on the "input" side and read from the "output"
//! side.  The byte stream is finite: the writer can end the input,
//! and then no more bytes can be written.
class ByteStream {
  private:
    // Your code here -- add private members as necessary.
    deque<char> _buffer;
    size_t _capacity;
    bool _end_input = false;
    size_t _written_bytes = 0;
    size_t _read_bytes = 0;
    // Hint: This doesn't need to be a sophisticated data structure at
    // all, but if any of your tests are taking longer than a second,
    // that's a sign that you probably want to keep exploring
    // different approaches.

    bool _error{};  //!< Flag indicating that the stream suffered an error.

  public:
    //! Construct a stream with room for `capacity` bytes.
    ByteStream(const size_t capacity);

    //! \name "Input" interface for the writer
    //!@{

    //! Write a string of bytes into the stream. Write as many
    //! as will fit, and return how many were written.
    //! \returns the number of bytes accepted into the stream
    size_t write(const std::string &data);

    //! \returns the number of additional bytes that the stream has space for
    size_t remaining_capacity() const;

    //! Signal that the byte stream has reached its ending
    void end_input();

    //! Indicate that the stream suffered an error.
    void set_error() { _error = true; }
    //!@}

    //! \name "Output" interface for the reader
    //!@{

    //! Peek at next "len" bytes of the stream
    //! \returns a string
    std::string peek_output(const size_t len) const;

    //! Remove bytes from the buffer
    void pop_output(const size_t len);

    //! Read (i.e., copy and then pop) the next "len" bytes of the stream
    //! \returns a string
    std::string read(const size_t len);

    //! \returns `true` if the stream input has ended
    bool input_ended() const;

    //! \returns `true` if the stream has suffered an error
    bool error() const { return _error; }

    //! \returns the maximum amount that can currently be read from the stream
    size_t buffer_size() const;

    //! \returns `true` if the buffer is empty
    bool buffer_empty() const;

    //! \returns `true` if the output has reached the ending
    bool eof() const;
    //!@}

    //! \name General accounting
    //!@{

    //! Total number of bytes written
    size_t bytes_written() const;

    //! Total number of bytes popped
    size_t bytes_read() const;
    //!@}
};

#endif  // SPONGE_LIBSPONGE_BYTE_STREAM_HH
```

> byte_stream.cc

```C++
#include "byte_stream.hh"

#include <algorithm>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : _buffer(0), _capacity(capacity){}

size_t ByteStream::write(const string &data) {
    size_t write_len = min(_capacity-_buffer.size(), data.length());
    for(size_t i=0; i<write_len; i++) _buffer.push_back(data[i]);
    
    _written_bytes += write_len;
    return write_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return string(_buffer.begin(), _buffer.begin()+min(len, _buffer.size()));
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t pop_size = min(len, _buffer.size());
    for (size_t i=0; i<pop_size; i++) {
        _buffer.pop_front();
    }
    _read_bytes += pop_size;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string read_data = peek_output(len);
    pop_output(len);
    return read_data;
}

void ByteStream::end_input() { _end_input = true; }

bool ByteStream::input_ended() const { return _end_input; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return _buffer.empty() && _end_input; }

size_t ByteStream::bytes_written() const { return _written_bytes; }

size_t ByteStream::bytes_read() const { return _read_bytes; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
```

**注意**：

- 类内成员变量命名规则，参考其中的`bool _error;`，所有变量应当以下划线`_`开头
- EOF的判断需要同时满足buffer为空且输入完毕

**结果验证**：

- 运行make check_lab0测试结果

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-06%2000.31.27.png" alt="截屏2022-10-06 00.31.27" style="zoom:50%;" />

