# YEGE

Yet another EGE library

This is a graphic/GUI library sharing similar interface with the EGE (Easy Graphics Engine) library.

Code based on [original implementation](http://misakamm.github.com/xege) (EGE 13.03).

Licensed GNU LGPLv3.

See [the change log](ChangeLog.md) (zh-CN) for details about modification of features & compatibility issues.

　　YEGE 库是和 EGE 图形库的一个接口类似的库。

　　代码来源于 EGE 13.03 。

　　遵循 GNU LGPLv3 发布。

　　特性修改和接口兼容性参见[更新日志](ChangeLog.md)（中文）。

# 目的

　　原作者已停止活动。当前仍在更新的 EGE 主要版本是 [wysaid](http://wysaid.org/) 维护的 [xege 分支(fork)](https://xege.org/ege-open-source)，其中发布的更新也被作为参考。

　　和原版 EGE 类似，YEGE 项目意图提供一个大体上兼容 [BGI(en-US)](https://en.wikipedia.org/wiki/Borland_Graphics_Interface) 的 C++ 图形开发库。

　　但是，和[原版 EGE 文档说明的](http://xege.org/manual/intro/index.htm)，本项目的侧重和目标有所不同：

* 通过设计的不同侧重，改善初学者的入门教育质量，避免扩大误导。
	* 不再强调*易用性*。
		* 易用性是相对的，而不是一个绝对的特征。
			* BGI 在历史上具有显著性的一个原因是易用，不表示在现代相比其它候选（如 [SDL](https://en.wikipedia.org/wiki/Simple_DirectMedia_Layer)），仍然足够显著。
		* 并且，这很大程度取决于用户的使用库的目的目的。
			* BGI 风格只是在移植历史遗漏代码时，提供了一个易用的候选。
			* 但是，本项目的设计不（那么）强调兼容性，对简化移植来说，是不（那么）易用的。
		* 易用可能来自于简单，简单可能来自于功能不全面。
			* 相对 GL/Vulkan/Direct3D 乃至 Cairo/Skia/Direct2D 等工业级的图形库，BGI 的原始 API 堪称*简陋*。
			* 基于 BGI 的设计并不容易扩展到现代的图形栈（尤其是难以全面利用硬件加速），因此当代工业级的图形库的设计风格有一些显著的不同。
	* 在提供传统 C 语言 API 的同时，从一开始就正确强调认识 C++ 和 C 的差异，以及类似设计作为图形库的历史局限性。
		* 这对初学者是**非常重要**的，特别是一些连 C 和 C++ 语言都不甚了解的入门者。
			* 实际上，包括 EGE 等在内的许多 C 语言教学环境的 BGI 兼容库已经依赖了 C++ ，在纯 C 语言环境不可用。
			* 因为学习上的困难和欠缺实用意义，这里的 API 仍然没有强调使用现代 C++ 。
				* 须知，[即便是 C++ ，也不是专业人士需要考虑的首选入门姿势](https://github.com/FrankHB/pl-docs/blob/master/zh-CN/introduction-to-learning-computer-languages.md)。
		* 对专业人士，学习兼容 BGI 这些[基本公认过时的古董](https://www.zhihu.com/question/403035995)通常是不必要的；但可以作为工作之外的参照。
			* 而对入门者，除了考古以外，更多的要点或许是提供材料，对比以后学习的更现代、更复杂的图形 API ，习惯如何迁移*范式(paradigm)* 。
	* 仅提供基本的图形库功能，引导用户向更现代的方案过渡。
		* 许多非专业人士不够理解图形库和图形用户界面(GUI, graphical user interface) 的差异。
			* 图形库不处理 UI ，特别是交互的部分。
			* 对图形工具集(toolkit) 而言，GUI 仅仅是便于实现附带的图形应用的便利接口，而非必要 API ，更不是重点。
		* 事实上，即便应当（必须）是足够专业的 [ISO/IEC JTC1/SC22/WG21](https://isocpp.org/std/the-committee) SG13 ，也存在混用图形和图形界面外延的混乱。
			* 以至于早期名为 2D 图形时，[议题中还混入了用户输入](https://groups.google.com/a/isocpp.org/g/sg13/c/-eF9DCAu9Sc/m/Gseg64qPP_YJ)。
			* 从早年的 2D 图形到后来的人机交互(HMI, human-machine interaction) ，虽然有改进，总体上是[失败](https://www.reddit.com/r/cpp/comments/89q6wr/sg13_2d_graphics_why_it_failed/)的。
* 通过具体实现，提供一种使用现代 C++ 满足现有 API 的工程迁移示范。
	* 在不改变或很少改变 C 风格为主的 API 的同时，其实现可以有巨大的调整。
	* 用户通过学习这些改动以掌握一些对设计上相对明确的旧有项目的实现改造和迁移技术。

　　基于以上目的，相对 EGE ，进一步移除其中最显著过时的 BGI 兼容 API 。

# 开发分支

　　项目的存储库包含两个主要分支：

* `master` ：主分支。
	* 这是为项目长期目标的主要分支。
	* 引入 [YSLib](https://osdn.net/projects/yslib) 作为必要依赖。
* `traditional` ：传统分支。
	* 这是和原版 EGE 以及其它 BGI(en-US) 实现的主要的 API 兼容的开发分支。
	* 这也是当前发布版本来源的分支。
	* 当前作为主分支而基线不定期合入主分支。部分仅在主分支使用的代码也在发布中保留。

# 文档

　　当前只提供有限的文档，暂不包含 API 文档。

## 接口约定

　　头文件中，被 `<ege.h>` 包含的 `ege` 目录下的文件是内部同文件，其文件名和内容 EGE 不保证兼容，不提供隐含的相互包含保证。

　　具体 EGE API 可通过这些头文件提供。不指定具体 API 通过 `ege` 目录中的哪些 API 提供。

　　`EGE` 起始的标识符和命名空间 `ege` 被保留。

　　可在命名空间中提供的 API 声明都在 `namespace ege` 中，只有 BGI 兼容头文件 `graphics.h` 同时在全局命名空间中引入声明。

　　关于具体 API ，可参照 [EGE 手册](http://xege.org/manual/)，对照[更新日志](ChangeLog.md)中关于不兼容改动的说明。

# 设计计划

## 非兼容特性

　　以下 [wysaid/xege](https://github.com/wysaid/xege) 特性在当前设计中没有计划支持：

* 新增 API ：
	* 带有不同字符串参数的[函数 `ege_drawtext`](https://github.com/wysaid/xege/pull/20) 。
	* [wysaid/xege pull request 25](https://github.com/wysaid/xege/pull/25) 中的部分特性：
		* 初始化选项 `INIT_UNICODE` ，可由使用者选择创建 Unicode 窗口。
		* 函数 `w2mb` 。
	* [wysaid/xege pull request 32](https://github.com/wysaid/xege/pull/32) 中的特性：
		* 函数 `mb2w` 。
	* [wysaid/xege pull request 82](https://github.com/wysaid/xege/pull/82) 中的部分特性：
		* 函数 `resize_f` 。
* [wysaid/xege pull request 29](https://github.com/wysaid/xege/pull/29) 中的部分特性：
	* 移除宏 `RGBTOBGR` 。
		* YEGE 在命名空间 `ege` 保留同名函数。
* [检查函数 `getimage` 的参数](https://github.com/wysaid/xege/pull/35)。
	* YEGE 使用仅在调试模式生效的断言。
* 不同的构建环境：
	* [使用 CMake 作为构建系统](https://github.com/wysaid/xege/pull/10)。
	* 基于自动配置检查的[选择性构建](https://github.com/wysaid/xege/commit/29808044ffac17cf1c2271609fb8a64816bd5f0d)。
	* [持续集成](https://github.com/wysaid/xege/pull/38)。

　　以下 [wysaid/xege](https://github.com/wysaid/xege) 特性在当前设计中暂不支持，可能会在以后支持：

* [wysaid/xege pull request 21](https://github.com/wysaid/xege/pull/21) 中的部分特性：
	* 支持[加载资源中 PNG 文件](https://github.com/wysaid/xege/pull/21/commits/eaba380a3b07d46263090a7e5064571ffc5af8d7)。
* [wysaid/xege pull request 22](https://github.com/wysaid/xege/pull/22) 和 [wysaid/xege pull request 23](https://github.com/wysaid/xege/pull/23) 中的特性。
* [wysaid/xege pull request 25](https://github.com/wysaid/xege/pull/25) 中的部分特性：
	* 函数 `seticon` 支持通过传入图标资源 ID 设置 EGE 窗口图标。
	* 允许在函数 `initgraph` 之前调用函数 `setcaption` 或 `seticon` 。
* [wysaid/xege pull request 33](https://github.com/wysaid/xege/pull/33) 中的特性。

　　以下 [wysaid/xege](https://github.com/wysaid/xege) 特性具有类似但不同的设计和实现：

* [将 `EgeControlBase` 移出 `ege.h`](https://github.com/wysaid/xege/pull/24) 。
	* YEGE 中已在不同的头文件。
	* 因为其它被间接包含的头文件也不在 `ege.h` 中，仍需 `ege/` ，YEGE 不支持单独分发 `ege.h` 。
* [部分重构 `IMAGE` 类](https://github.com/wysaid/xege/pull/28)。
	* 对[相关初始化问题的修复](https://github.com/wysaid/xege/issues/2)仍然在逻辑上存在缺陷。
		* YEGE 重构的方式不同，不支持初始化前使用资源。
	* 调整大小的具有不同的作用（对函数 `resize` 可见）：
		* [wysaid/xege 修改不初始化背景](https://github.com/wysaid/xege/commit/16cfad18cc847e7c525975c1f060a1bf0b18b9f8)，但[之后又被撤销](https://github.com/wysaid/xege/pull/82)。
		* YEGE 调整对应大小时使用 `ScreenBuffer` ，内容未指定。
* [wysaid/xege pull request 34](https://github.com/wysaid/xege/pull/34) 中的特性和对 BGI 兼容的[函数 `initgraph` 重载的参数类型 `char*` 修改为 `const char*`](https://github.com/wysaid/xege/commit/85d841ffc03496ae7fb71e75ae33199c6687f3c2) ：
	* 因为 YEGE 不支持 BGI 函数重载，没有需要修复的初始化行为。
	* YEGE 不使用 `-1` 作为模式（使用 `-1` 作为模式可能会影响初始化状态，未在 wysaid/xege 文档中说明）；这也[在之后被撤销](https://github.com/wysaid/xege/commit/85d841ffc03496ae7fb71e75ae33199c6687f3c2)。

