# 概要

YEGE 以 [misakamm 的 xege](http://github.com/misakamm/xege) 为基础修改，提供兼容特性。

# 版本历史

## 主分支版本

### API 修改

* 函数 `getkey` 支持 `WM_CHAR` 消息。
	* 参见 [wysaid/xege pull request 3](https://github.com/wysaid/xege/pull/3) 。
* 修复 `putimage_`* 系列函数剪裁区域和坐标变换的问题。
	* 参见 [wysaid/xege pull request 5](https://github.com/wysaid/xege/pull/5) 。
	* 参见 [wysaid/xege pull request 9](https://github.com/wysaid/xege/pull/9) 。
* 颜色转换函数添加 `ynothrow` 。
* 修复函数 `RGBTOBGR` 的参数类型（自从 19.01 ）。
* 修改 `color_t` 的格式：交换红色和蓝色分量。
	* 撤销 14.01 的修改，和原始 misakamm/xege 的 `color_t` 一致，而不再和 `::COLORREF` 一致。
	* 同时修改函数 `EGERGB` 的实现，解决和其它函数的不一致问题。
	* 修改后的格式和 `YSLib::Pixel` 在 Win32 上的实现以及 [wysaid/xege pull request 12](https://github.com/wysaid/xege/pull/12) 中的像素格式保持一致，存储格式都为 BGRA8888 。
* 函数 `clearviewport` 使用背景颜色填充。
	* 参见 [wysaid/xege pull request 12](https://github.com/wysaid/xege/pull/12) 。
* 修复函数 `getch` 阻塞不返回（自从 19.01 ）。
* 修复函数 `setcolor` 、`setlinestyle` 和 `setlinewidth` 并简化实现。
	* 参见 [wysaid/xege pull request 13](https://github.com/wysaid/xege/pull/13) 。
* 修复函数 `ege_setalpha` 对无效的 `IMAGE`（未成功初始化的图像）缺少检查。
	* 另见 [wysaid/xege pull request 16](https://github.com/wysaid/xege/pull/16) 。
* 优化函数 `ege_setalpha` 的实现。
* 添加小键盘符号键的键码。
	* 参见 [wysaid/xege pull request 17](https://github.com/wysaid/xege/pull/17) 。

### 非外部依赖项代码风格和格式

* 函数参数列表 `(void)` 修改为 `()` 。

### 外部依赖项全局名称

具有以下所列前缀的外部依赖项全局名称在使用时附加前缀 `::` ：

* `ExtC`

## 19.01

YEGE 19.01 以 YEGE 14.01 为基础修改。

### 限制实现需求

依赖 [YSLib](https://bitbucket.org/FrankHB/yslib)（见下文）时，要求 MinGW G++ 5.3.0 或以上。YSLib 提供的库 YBase 和 YFramework 的 API 需要满足最低实现支持。以后的版本可能提升实现的支持要求。

只测试 MinGW-W64（使用同 YSLib 项目相同的 [MSYS2](https://www.msys2.org/) 基础环境）。未测试 [MinGW.org](http://mingw.org/) 的运行时。

### 依赖项更改

* 当定义宏 `YEGE_Use_YSLib` 时，依赖 YSLib 。
* 移除 GDI+ 源代码，需要显式依赖 GDI+（默认 MinGW 已提供）；移除 `EGE_GDIPLUS` 宏。
* 移除 zlib 和 libpng 源代码和头文件，需要显式依赖库 `z` 和 `png16` 。
* 依赖 YSLib 时移除 `ole32` 、`oleaut32` 和 `uuid` ，否则移除 `imm32` 。

### API 修改

接口扩充：

* 在 `ege` 命名空间提供类型别名 `color_int_t` 和 `MonoType` 表示颜色转换整数类型和分量类型。

YSLib 兼容接口扩充（依赖 YSLib 时直接使用 YSLib 对应接口）：

* 提供和 YBase 兼容的伪关键字宏 `yconstfn` 、`yconstexpr` 和 `ynothrow` ，分别替换为 `constexpr` 、`constexpr` 和 `noexcept` 。
* 提供和 YBase 兼容的伪关键字宏 `yconstraint` 和 `yassume` 替换 `assert` 。
* 提供和 YBase 兼容的伪关键字宏 `yunseq` 。
* 提供和 YBase 一致实现的伪关键字宏 `yunused` 。
* 在 `ege` 命名空间提供和 YBase 一致实现的类型 `octet` 和 `byte` 。
* 在 `ege` 命名空间使用 `using` 引入智能指针类型和相关操作的声明，并提供 `std::make_unique` 的替代实现。
* 在 `ege` 命名空间提供和 YCLib 的 `platform::Deref` 和 `platform::Nonnull` 兼容的模板。
* 在 `ege` 命名空间提供和 YSLib 兼容的 `SPos` 和 `SDst` 类型。
* 提供和 YBase 一致实现的宏 `YPP_Empty` 、`YPP_Commma` 和 `YPP_Concat` 。
* 提供和 YCLib 兼容的宏 `YTraceDe`。
	* 空实现。
* 提供和 YSLib 一致实现的以下宏：
	* `PDefH` `PDefHOp`
	* `ImplExpr` `ImplRet` `ImplThrow` `ImplUnseq`
	* `TryExpr` `TryRet`
	* `CatchExpr` `CatchIgnore` `CatchThrow`
	* `DefDeCtor` `DefDelCtor`
	* `DefDeCopyCtor` `DefDelCopyCtor` `DefDeMoveCtor` `DefDelMoveCtor`
	* `DefDeCopyMoveCtor`
	* `DefDeDtor` `DefDelDtor` `ImplDeDtor`
	* `DefDeCopyAssignment` `DefDelCopyAssignment` `DefDeMoveAssignment` `DefDelMoveAssignment` `DefDeCopyMoveAssignment` `DefDeCopyMoveCtorAssignment`
	* `DefCvt`
	* `DefNeg` `DefBoolNeg`
	* `DefPred` `DefGetter` `DefGetterMem` `DefSetter`
	* `DefSwap`
* 提供和 YCLib 兼容的宏 `YAssert`。
	* 替换为 `yassume` 表达式。
* 在 `ege` 命名空间提供和 YSLib 兼容的 `min` 和 `max` 模板。
	* 非 YSLib 使用 `std` 的对应名称代替，不保证 `constexpr` 。
* 在 `ege` 命名空间提供和 YSLib 兼容的 `HalfDifference` 、`IsInInterval` 和 `IsInOpenInterval` 模板。
* 在 `ege` 命名空间提供和 `YSLib::Drawing` 兼容的模板 `GBinaryGroup` 、类型 `Point` 、`Vec` 、`Size` 和 `Rect` 以及相关操作。
* 在 `ege` 命名空间提供和 `YSLib::Drawing` 二进制兼容的类型 `BitmapPtr` 和 `ConstBitmapPtr`。
	* 实现为 `color_int_t` 的指针。
* 提供和 YBase 兼容的宏 `YB_EXPECT` 、`YB_LIKELY` 和 `YB_UNLIKELY`。
	* 仅实现判断 GCC ，不确定版本。
* 在 `ege` 命名空间提供和 YCLib 兼容的类 `ScreenBuffer` ，但成员 `Premultiply` 、`UpdatePremultipliedTo` 和 `UpdateTo` 除外。

非向后兼容接口：

* 添加 `linestyletype` 和 `EGEMSG` 的构造函数，只支持默认构造和提供所有成员的初始化。
* 宏 `RGBTOBGR` 、`EGERGB` 、`EGERGBA` 、`EGEARGB` 、`EGEACOLOR` 、`EGECOLORA` 、`EGEGET_R` 、`EGEGET_G` 、`EGEGET_B` 、`EGEGET_A` 、`EGEGRAY` 、`EGEGRAYA` 和 `EGEAGRAY` 变更为 `ege` 命名空间内的函数。
* 移除枚举 `graphics_modes` 。
* 移除类 `Lock` ，使用 `std::lock_guard` 等代替。
* 移除类模板 `thread_queue` 的 `last` 和 `unpop` 函数。
* 函数 `drawlines` 、`fillpoly_gradient` 和 `inputbox_getline` 在分配存储失败时抛出 `std::bad_alloc` 异常。
* 函数 `inputbox_getline` 等加入断言。
* 移除函数 `api_sleep` 。使用 `::Sleep` 函数代替。
* 移除函数 `delay` 。使用 `ege_sleep` 函数代替。
* 移除函数 `setrendermode` 和枚举 `rendermode_e` 。（默认状态等价于 `setrendermode(RENDER_MANUAL)` 。原有代码使用 `RENDER_AUTO` 和 `RENDER_MANUAL` 渲染的效果相同，都不闪烁。）
* 移除兼容宏 `RGBtoGRAY` 、`RGBtoHSL` 、`RGBtoHSV` 、`HSLtoRGB` 和 `HSVtoRGB`，使用函数 `rgb2gray` 、`rgb2hsl` 、`rgb2hsv` 、`hsl2rgb` 和 `hsv2rgb` 代替。
* 枚举 `initmode_flag` 中枚举项 `INIT_DEFAULT` 的值固定为 `0x0`（枚举项 `INIT_RENDERMANUAL` 和 `INIT_WITHLOGO` 仅为兼容性保留）。
* 移除输入法消息处理。
* 允许主窗口循环接受消息 `WM_QUIT` 以退出。
* 移除 `getProcfunc` 和 `SetCloseHandler` 函数。
* 函数 `settarget` 返回类型恢复为 `void` 。
* 初始化函数 `initgraph` 移除兼容 [BGI](https://en.wikipedia.org/wiki/Borland_Graphics_Interface) 的重载，只接受 `size_t` 类型的主窗口宽和高（等于 0 时仍然表示屏幕宽和高）。
* 移除类型 `CALLBACK_PROC` 。
* 移除兼容宏 `getmaxx` 和 `getmaxy` ，使用函数 `getwidth` 和 `getheight` 代替。
* 移除枚举 `graphics_drivers` 、`message_event` 和 `message_mouse` 。
* 函数 `resize` 调用后状态未指定，不保证填充背景色。

非向后兼容接口（仅当依赖 YSLib 时）：

* `typedef` 类型名 `color_t` 从 `unsigned int` 改为 `YSLib::Drawing::PixelType` 。
* 使用 YCLib 提供的 `YSLib::Drawing::MonoType` 表示颜色分量。
* 使用 `YSLib::Drawing::PixelType::Integer` 表示颜色对应的整数类型。
	* 注意布局同 `::COLORREF` ，相对原有实现红色和蓝色分量交换，修复了 `<wingdi.h>` 中 RGB 等宏的参数红色和蓝色分量相反的问题。
* 函数 `savepng` 的表示是否使用 alpha 的参数被忽略。
* 颜色转换断言严格要求色调值在区间 [0, 360) 内。
* 越界访问图像页面时使用 `YTraceDe` 警告。

向后（源代码）兼容但不保证向前兼容原始实现的接口：

* `linestyletype` 和 `EGEMSG` 默认初始化所有成员。
* 函数 `ege_sleep` 支持多线程环境下使用。（实际原有实现可在多线程下使用但未在头文件中明确。）
* 提升 `fclock` 函数的精度。
* 随机数函数第一次调用时自动初始化随机数种子，不需要事先调用 `randomize` 函数。
* 窗口过程不依赖 `GWLP_USERDATA` 。
* 创建图像和图像页面操作取屏幕设备上下文替代全局图像页面状态。
* 设置图像大小不依赖公共 `API` 。
* 设置图像页面时检查越界并忽略越界访问。

兼容接口和实现调整：

* 替换以下 Windows SDK 类型：
	* `VOID` → `void` 。
	* `PVOID` → `void*` 。
	* `LPVOID` → `void*` 。
	* `DWORD` → `unsigned long` 。
	* `PDWORD` → `unsigned long*` 。
	* `DWORD_PTR` → `::ULONG_PTR` 。
* 所有同名 `typedef` 结构体名称或 `typedef` 匿名结构体调整为非 `typedef` 结构体类型名。
* 宏 `EGEAPI` 调整至返回值前；移除函数形参中不必要的 `const` 。

修复错误实现：

* 修复绘制时遗漏调用 `::EndPaint`（此 bug 在 EGE 15.04 修复）。
* 修复退出时遗漏清理 GDI+ 。
* 修复初始化图形时设置背景读取未初始化的缓冲区。
* 修复初始化图形时未初始化 `linestyletype` 被读取。
* 修复图像页面未分配存储被访问（此 bug 导致 Windows 10 1809 下崩溃，[在 EGE 19.01 修复](https://github.com/misakamm/xege/pull/8/commits/6a2d8bb7236356a456d0a51b1d51c764adf10cdc)）。
* 修复图像页面未初始化存储被 `ege::window_setviewport` 读取。
* 修复未初始化的消息成员被读取。
* 修复未初始化的全局状态被读取。
* 修复退出时遗漏清理图像页面。
* 修复函数 `flushkey` 和 `flushmouse` 调用可能被中断导致队列非空。
* 修复函数 `inputbox_getline` 未检查分配存储失败。

实现注记：

* 返回指针的内联函数 `CONVERT_IMAGE` 和 `CONVERT_IMAGE_CONST` 改用返回引用的非内联函数 `cimg_ref` 和 `cimg_ref_c` 替代。
* 移除不符合标准的保留名称 `_RGBtoHSL` 和 `_HSLtoRGB` 的使用。
* 使用 C++11 标准库 `<thread>` 提供的 `std::this_thread::sleep_for` 代替 `::HTIMER` 和 `::Sleep` 实现延时。
* 使用 C++11 标准库 `<chrono>` 和 YSLib 提供的包装接口实现其它时间操作。
* `thread_queue` 类模板使用 `std::mutex` 、`std::lock_guard` 和 `std::deque` 实现。
* 提升 `flushkey` 和 `flushmouse` 函数的性能。
* 避免直接使用 `malloc` 和 `free` 。
* 拆分宿主环境主窗口和控件的窗口过程。
* 简化绘图 API 实现。
* 使用 YSLib 兼容的 `ScreenBuffer` 实现类 `IMAGE` 。

实现注记（仅当依赖 YSLib 时）：

* 使用 `YSLib::Drawing` 实现函数 `rgb2hsl` 、`hsl2rgb` 、`rgb2hsv` 和 `hsv2rgb`（不保证精度完全一致）。
* 使用 `YSLib::Adaptor::Image` 模块提供的接口代替 OLE 和 libpng 实现图像读取操作。
* 使用 `YCLib::YCommon` 模块提供的接口和 `std::unique_ptr` 简化编码转换并提供异常安全。
* 使用 `YSLib::Drawing::HBitmap` 实现 BMP 和 PNG 图像保存。

### 实现质量

EGE 库中加入 `-Wall -Wcast-align -Wconditionally-supported -Wctor-dtor-privacy -Wdeprecated -Wdeprecated-declarations -Wextra -Wformat=2 -Winvalid-pch -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wmultichar -Wno-format-nonliteral -Wnon-virtual-dtor -Wpacked -Wredundant-decls -Wsign-promo -Wstrict-null-sentinel -Wtrampolines -Wzero-as-null-pointer-constant -pedantic-errors` 保证无错误和警告。

依赖 YSLib 时，EGE 库中加入 `-Wfloat-equal` 保证无警告。

EGE 代码 `-Wold-style-cast` 保证无警告（因为依赖库头文件问题没有加入此选项）。

### 非外部依赖项代码风格和格式

主函数不使用冗余形参和返回语句。

### 外部依赖项全局名称

具有以下所列前缀的外部依赖项全局名称在使用时附加前缀 `::` ：

* `HMENU`
* `LRES`
* `OleLoadPicture`
* `SetLayeredWindowAttributes`
* `SetStretch`

扩展 `OleLoadPicturePath` 到 `OleLoadPicturePath` 前缀的外部依赖项全局名称在使用时前缀 `::` 。

以下过时的前缀的外部依赖项全局名称在使用时不再需要附加前缀 `::` ：

* `HEVENT`
* `HRGN`
* `LARGE_INT`
* `ImmSetCompositionWindow`
* `Query`
* `ResumeT`
* `TCHA`
* `wsprintfW`

添加以下所列外部依赖项全局名称在使用时不再需要附加前缀 `::` ：

* `BYTE`

过时的外部依赖项全局名称在使用时不再需要附加前缀 `::` ：

* `access`

## 14.01

YEGE 19.01 以 EGE 13.03 为基础修改。

### 增加 IDE 支持

Code::Blocks 。

### 限制实现需求

限 MinGW G++ 4.8.0 或以上，需要 `-std=c++11` 支持。使用了 `std::thread` ，需要 POSIX 线程模型。

（其它版本未测试，但至少需要 `-std=c++0x` 的 `auto` 、`nullptr` 、lambda 表达式和初始化列表等 C++11 特性支持，且若在 Code::Blocks 项目使用，需要去除 `debug` 的 `-Og` 选项。）

只测试 MinGW-W64 。未测试 MinGW.org 的运行时。

### 依赖项更改

* 计划依赖 YBase 的 `ydef.h`（当前临时移除）。
* 移除 GDI+ 源代码，需要显式依赖 GDI+（默认 MinGW 已提供）；移除 `EGE_GDIPLUS` 宏。
* 移除 zlib 和 libpng 源代码，需要显式依赖库 `z` 和 `png16` 。

### API 修改

非向后兼容接口：

* 入口统一为 `::main` 。
* 编译为 DLL 无需定义宏。
* 移除 `SHOW_CONSOLE` 宏。若需要控制台，在 Code::Blocks 选项内选择，或确保编译命令行无 `-mwindows` 。
* 移除宏 `EGE_DEPRECATED` 。
* 移除 EGE 应用程序启动时的 Logo 显示。
* 移除所有标记为 deprecated 的接口。
* 移除宏 `Sleep` 。
* 函数原型 `PVOID EGEAPI getProcfunc();` 更改为 `EGEAPI ::WNDPROC getProcfunc();` 。
* 移除类模版 `Array` 和 `Set` 。
* 移除宏 `_Cdecl` 。
* 移除 `PIMAGE` 类型，用 `IMAGE*` 替代；取消原 `PIMAGE` 类型表示图像页面的用法，仅保留空指针值表示目标图像页面。
* 移除宏 `INITGRAPH(x, y)` 、`INITGRAPH3(x, y, f)` 和 `EGECTRL_INITEND()` 。
* 函数 `showmouse` 和 `mousemsg` 的参数/返回值使用 `bool` 代替 `int` 。
* 宏 `CTL_`* 的定义不再被默认包含。若需使用，另行包含 `<ege/mac.h>` 或 `<ege/ctl.h>` 等。
* 移除函数 `attachHWND` 以避免潜在的死锁。
* 函数 `setrendermode` 无效，默认状态统一为 `setrendermode(RENDER_MANUAL)` ；仅为兼容性保留。
* 移除类型 `LPCALLBACK_PROC` 、`LPMSG_KEY_PROC` 、`LPMSG_MOUSE_PROC` 、`MSG_KEY_PROC` 和 `MSG_MOUSE_PROC` ，其中 `LPCALLBACK_PROC` 可用 `CALLBACK_PROC*` 代替。

向后（源代码）兼容但不保证向前兼容原始实现的接口：

* `delay_fps` 和 `delay_jfps` 仅保留 `double` 重载版本。
* 部分函数（包括 `mousepos` 等）加入断言检查参数。
* 绘图环境设置、控件和音乐 API 补充遗漏的 `EGEAPI` 以便作为 DLL 使用，但 `fps` 类仅头文件实现而例外。
* 控件基类 `egeControlBase` 的析构函数保证为 `virtual` 。
* 宏 `EGEAPI` 支持显式导入和导出。

兼容接口和实现调整:

* 替换以下 Windows SDK 类型：
	* `CHAR` → `char` 。
	* `WCHAR` → `wchar_t` 。
	* `LPSTR` → `char`* 。
	* `LPCSTR` → `const char*` 。
	* `LPWSTR` → `const wchar_t*` 。
	* `LPCWSTR` → `const wchar_t*` 。
* 所有同名 `typedef` 结构体名称或 `typedef` 匿名结构体调整为非 `typedef` 结构体类型名。
* 宏 `EGEAPI` 调整至返回值前；移除函数形参中不必要的 `const` 。

实现注记：

* 随机数改用 C++11 标准库 `<random>` 实现。
* 移除的 `Array` 和 `Set` 模板改用 `std::vector` 和 `std::set` 代替。
* 移除 `SWAP` 宏，使用 C++ 标准库 `std::swap` 代替。
* 移除 `_GRAPH_LIB_BUILD_` 宏。
* 移除 `FLOAT_EPS` 宏，使用更精确的标准库 `<cfloat>` 中的宏 `FLT_EPSILON` 代替。
* 移除 `typedef` 名称 `unint32` ，使用 `<cstdint>` 的 `std::uint32_t` 代替。
* 移除冗余 `IFATODOB` 和 `IFNATODOB` 宏。
* 移除冗余 `typedef` 名称 `PLONG_PTR` 、`PULONG_PTR` 、`ULONG_PTR` 、`PDWORD_PTR` 和 `POINTER_SIZE`（部分已在 Windows SDK 头文件中定义）。
* 移除 `enum dealmessage_update` 类型，使用 `bool` 代替。
* 移除 `_DEBUG` 宏。
* 移除 `ERROR_SUCCESS` 宏，使用字面量 `0` 代替。
* 移除 `GRADIENT_`* 、`__int3264` 宏 、`typedef` 名称（ `LONG_PTR` 和 `DWORD_PTR` ）和函数 `GradientFill` 的声明，使用 MinGW 头文件。
* 移除 `count_ptr` 模板（当前未使用，必要时可用 `std::shared_ptr<void>` 代替）。
* 移除 `ASSERT_TRUE` 宏。
* 移除非接口函数 `detectgraph` 、`message_addkeyhandler` 和 `message_addmousehandler` 的孤立实现。
* 使用 `std::thread` 代替 WinAPI 操作线程。
* 移除宏 `CONVERT_IMAGE_END` 。
* 宏 `CONVERT_IMAGE_F` 和 `CONVERT_IMAGE_F_CONST` 调用替换为 `CONVERT_IMAGE` 和 `CONVERT_IMAGE_CONST`；移除宏 `CONVERT_IMAGE_F` 和 `CONVERT_IMAGE_F_CONST` 。
* 移除 `setrendermode` 实现所需的计时器例程；主窗口消息循环不响应 `WM_TIMER` 。
* 取消手动设置 `exit_window` 状态，改用 UI 线程状态表示；移除无用的 `exit_flag` 。
* 类型 `PDWORD` 用 `DWORD*` 代替。
* 类 `IMAGE` ：修复复制构造；新增交换、转移构造和统一赋值操作；移除成员初始化检查；封装部分成员保证不可在外部修改。
* 宏 `CONVERT_IMAGE` 和 `CONVERT_IMAGE_CONST` 改用内联函数实现，且当输入除空指针值外返回原值。
* 命名空间作用域静态函数和对象去除 `static` 并使用未命名命名空间代替。

### 实现质量

EGE 库中加入 `-Wall -Wextra -Wmissing-include-dirs -Wzero-as-null-pointer-constant -pedantic-errors` 保证无错误和警告。

其它项目中加入 `-Wall -Wextra -pedantic` 保证无警告。

### 非外部依赖项代码风格和格式

`{` 之前不邻接空格。

指针和空指针的相等比较用 `!` 操作符代替，不等比较使用 `bool` 转换代替。

尽可能使用 `{}` 代替 `NULL` 或其它形式的空指针常量；无法使用 `{}` 的用 `nullptr` 代替。

字面量 `false` 尽可能用 `{}` 代替。

### 标准库使用

标准库实体名称保证 `std::` 起始或使用 `using` 声明/指示。

使用 C++ 标准库头文件而不是 C 标准库头文件。

具有以下所列前缀的外部依赖项全局名称在使用时附加前缀 `::` ：

* `_wf`
* `Arc`
* `ClientToScreen`
* `CloseHandle`
* `Create`
* `DWORD`
* `DefWindowProcW`
* `Delete`
* `DestroyWindow`
* `DispatchMessage`
* `DrawText`
* `Ellipse`
* `ExitProcess`
* `ExtFloodFill`
* `FillRect`
* `FloodFill`
* `GRADIENT_TRIANGLE`
* `GetClientRect`
* `GetCurrent`
* `GetDeviceCaps`
* `GetLastError`
* `GetM`
* `GetObject`
* `GetSystemMetrics`
* `GetTextExtentPoint32`
* `GetWindow`
* `Global`
* `HANDLE`
* `HB`
* `HFONT`
* `HGLOBAL`
* `HDC`
* `HICON`
* `HINSTANCE`
* `HMODULE`
* `HPEN`
* `HRESULT`
* `IID_IPicture`
* `IPicture`
* `LOGBRUSH`
* `LOGFONT`
* `LOGPE`
* `LONG`
* `LPARAM`
* `LoadCu`
* `LoadI`
* `LoadRes`
* `MCI_STATUS_PARMS`
* `MCIE`
* `MessageBox`
* `MoveW`
* `MulD`
* `MultiB`
* `OLE`
* `OleLoadPicturePath`
* `POINT`
* `Pie`
* `Poly`
* `Post`
* `RECT`
* `RegQueryValueExA`
* `ReleaseDC`
* `ScreenToClient`
* `Select`
* `SetBk`
* `SetCapture`
* `SetCursor`
* `SetEvent`
* `SetT`
* `SetViewportOrgEx`
* `SetWindow`
* `ShowWindow`
* `Sleep`
* `StretchBlt`
* `TRIVERTEX`
* `TranslateMessage`
* `UINT`
* `ULONG`
* `WPARAM`
* `WaitForSingleObject`
* `lstrc`
* `lstrl`
* `mciSend`
* `png_`

以下所列外部依赖项全局名称在使用时附加前缀 :: ：

* `HWND`
* `MSG`
* `access`

