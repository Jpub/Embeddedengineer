#include <linux/module.h>         // カーネルモジュールをロードするためのヘッダ
#include <linux/init.h>           // __init, __exit, といったマクロを利用するためのヘッダ

/**
 * [hello_init モジュールロード時に呼ばれます。]
 * @return  [0:正常終了]
 */
static int __init hello_init(void)
{
  printk(KERN_INFO "Hello World\n");
  return 0;
}

/**
 * [hello_exit モジュールアンロード時に呼ばれます]
 */
static void __exit hello_exit(void)
{
  printk(KERN_INFO "Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
