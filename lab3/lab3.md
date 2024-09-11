多个c代码中有相同的MY_MMult函数，怎么判断可执行文件调用的是哪个版本的MY_MMult函数？是makefile中的哪行代码决定的？
> makefile的链接规则末尾的目标文件会覆盖前面的，具体由以下代码决定

```makefile
$(LINKER) $(OBJS) $(LDFLAGS) -o $@
```
<br>

性能数据_data/output_MMult0.m是怎么生成的？c代码中只是将数据输出到终端并没有写入文件。
> makefile中包含echo指令，将性能数据写入文件