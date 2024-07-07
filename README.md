#### **Bitusk**

>***A c++ bitterrent downloader***
>
> For drilling my c++ skills.
```
.----. .-. .---. .-. .-. .----..-. .-.
| {}  }| |{_   _}| { } |{ {__  | |/ / 
| {}  }| |  | |  | {_} |.-._} }| |\ \ 
`----' `-'  `-'  `-----'`----' `-' `-'
```

Used Libraries:
1. catch2, lightweight unit test framwork.
2. asio, provide async network io.

Progress:

+ bt文件解析
+ tracker服务器连接
    >TODO: support UDP server
+ peer data exchange 30%
+ datacache 80%
    >TODO: support other method to write to disk
+ message generate and parse
+ logger 100%
+ signal control 0%


[开发笔记:代码细节与结构上的考量](https://github.com/sunyiynus/bitusk/blob/master/docs/%E8%AE%BE%E8%AE%A1%E6%96%87%E6%A1%A3.md)


改进

针对DRUSS(dependable, reliable, usable, safe, secure)标准的一些分析。


可以针对Dependable作出的改进：

ref.
1. [The BitTorrent Protocol Specification](https://www.bittorrent.org/beps/bep_0003.html)
2. [Building a BitTorrent client from the ground up in Go](https://blog.jse.li/posts/torrent/)


roadmap
1. 分析要做那些部分
2. 分析技术难题
3. 搭建平台