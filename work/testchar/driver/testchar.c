#include <linux/init.h>           // __init, __exit, といったマクロを利用するためのヘッダ
#include <linux/module.h>         // カーネルモジュールをロードするためのヘッダ
#include <linux/device.h>         // kernel Driver Modelヘッダ
#include <linux/kernel.h>         // kernelのマクロや関数ヘッダ
#include <linux/fs.h>             // Linux file systemのためのヘッダ
#include <linux/uaccess.h>        // データをユーザ空間にコピーするためのヘッダ

#define  DEVICE_NAME "testchar"    // /dev配下に作成するデバイスファイル名
#define  CLASS_NAME  "sample"      // /sys/class配下に作成するクラス名

static int    majorNumber;        // メジャー番号 -- 動的に割り当てられる
static struct class*  testcharClass  = NULL; // sysfsのクラス
static struct device* testcharDevice = NULL; // このデバイスドライバの管理情報
static int    numberOpens = 0;    // デバイスファイルがopenされた数を保持する
static short  size_of_message = 0;// ユーザ空間から受け取ったデータサイズを保持
static char   message[16] = {0};  // ユーザ空間から受け取ったデータを保持

/* プロトタイプ宣言 */
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/*
   デバイスファイルへのシステムコールハンドラです。
   file_operations構造体のメンバに対して、関数ポインタを登録します。
   例えば、アプリケーションでopen()を呼ぶと、dev_open()が呼ばれます。
*/
static struct file_operations fops =
{
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};

/**
 * [dev_open デバイスファイルがopen()されるたびに呼ばれます。
 *  ドライバロード後、何回open()されたかを出力します。]
 * @param  inodep [inode（Linux file systemのデータ構造）オブジェクトへのポインタ（不使用）]
 * @param  filep  [fileオブジェクトへのポインタ（不使用）]
 * @return        [0:正常終了]
 */
static int dev_open(struct inode *inodep, struct file *filep){
  numberOpens++;
  printk(KERN_INFO "testchar: Device has been opened %d time(s)\n", numberOpens);
  return 0;
}

/**
 * [dev_release デバイスファイルがclose()/release()されると呼ばれます。]
 * @param  inodep [inode（Linux file systemのデータ構造）オブジェクトへのポインタ（不使用）]
 * @param  filep  [fileオブジェクトへのポインタ（不使用）]
 * @return        [0:正常終了]
 */
static int dev_release(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "testchar: Device successfully closed\n");
  return 0;
}

/**
 * [dev_read デバイスファイルがread()されると呼ばれます。
 *  write()で書かれたデータをユーザへ返します。]
 * @param filep fileオブジェクトへのポインタ（不使用）
 * @param buffer デバイスから読み出したデータをユーザへ渡すためのバッファ
 * @param len bufferの長さ（不使用）
 * @param offset 読み出し位置のオフセット（不使用）
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
  int error_count = 0;
  /* ユーザ空間にデータをコピーします。エラー数が0であれば成功です。 */
  error_count = copy_to_user(buffer, message, size_of_message);

  if (error_count==0){
    printk(KERN_INFO "testchar: Sent %s(%d characters) to the user\n", buffer, size_of_message);
    /* 受信したデータサイズをクリアして、0をユーザに返します。 */
    return (size_of_message=0);
  }
  else {
    printk(KERN_INFO "testchar: Failed to send %d characters to the user\n", error_count);
    return -EFAULT;
  }
}

/**
 * [dev_write デバイスファイルがwrite()されると呼ばれます。
 *  書かれたデータをmessageに保持します。]
 * @param  filep  [fileオブジェクトへのポインタ（不使用）]
 * @param  buffer [デバイスに書き込むデータを格納したバッファ]
 * @param  len    [bufferに格納された文字列の長さ（不使用）]
 * @param  offset [書き出し位置のオフセット（不使用）]
 * @return        [受け取ったメッセージのサイズ]
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  sprintf(message, "%s", buffer);
  size_of_message = strlen(message);
  printk(KERN_INFO "testchar: Received %s(%zu characters) from the user\n", buffer, len);
  return len;
}

/**
 * [testchar_init ドライバロード時にデバイスドライバをカーネルに登録します。
 * @return  [0:正常終了]
 */
static int __init testchar_init(void){
  printk(KERN_INFO "testchar: Initializing the testchar\n");

  /* このデバイスに動的なメジャー番号を割り当てます。 */
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber<0) {
    /* エラー処理 */
    printk(KERN_ALERT "testchar failed to register a major number\n");
    return majorNumber;
  }
  printk(KERN_INFO "testchar: registered correctly with major number %d\n", majorNumber);

  /* sysfsにデバイスクラスを登録します。 */
  testcharClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(testcharClass)){
    /* エラー処理 */
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(testcharClass);
  }
  printk(KERN_INFO "testchar: device class registered correctly\n");

  /* デバイスドライバを登録します。 マイナー番号は0で固定しています。
    複数デバイスを制御する場合は、動的にマイナー番号を登録します。*/
  testcharDevice = device_create(testcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(testcharDevice)){
    /* エラー処理 */
    class_destroy(testcharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(testcharDevice);
  }
  printk(KERN_INFO "testchar: device class created correctly\n");
  return 0;
}

/**
 * [testchar_exit 初期化時に確保・登録した資源を、解放・登録解除します。]
 */
static void __exit testchar_exit(void){
  device_destroy(testcharClass, MKDEV(majorNumber, 0));
  class_unregister(testcharClass);
  class_destroy(testcharClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "testchar: Goodbye\n");
}

module_init(testchar_init);
module_exit(testchar_exit);

MODULE_LICENSE("GPL");
