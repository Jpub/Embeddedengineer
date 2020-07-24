(1) ドライバのコンパイル・ロード
$ cd driver
$ make
$ sudo insmod testchar.ko
$ lsmod | grep testchar
testchar        16384  0

(2) アプリケーションのコンパイル
$ cd app
$ make

(3) アプリケーションの実行
$ ./test
Starting device test code example...
Type in a short string to send to the kernel module:

※任意の数字を入力して下さい（16桁まで）
123
Writing message to the device [123].
Press ENTER to read back from the device...

※ドライバが入力した数字を保持します。
※ENTERキーを押すと、ドライバが保持している値を取得します。

Reading from the device...
The received message is: [123]
End of the program


(4) ドライバのログ確認
$ dmesg | tail -n 20
[1024236.474242] testchar: Device has been opened 1 time(s)
[1024288.198301] testchar: Received 123(3 characters) from the user
[1024338.179631] testchar: Sent 123(3 characters) to the user
[1024338.179741] testchar: Device successfully closed

※上記4行がアプリケーション実行時のログです。


(5) ドライバをアンロード
$ sudo rmmod testchar
