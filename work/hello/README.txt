(1) ドライバのコンパイル・ロード
$ make
$ sudo insmod hello.ko
$ lsmod | grep hello
hello        16384  0 ※ドライバがロードされていることを確認します。

(2) 動作確認
$ dmesg | tail
[2387251.240773] Hello World

(3) ドライバのアンロード
$ sudo rmmod hello

(4) 動作確認
$ lsmod | grep hello ※何も表示されない＝ドライバがアンロードされていることを確認します。
$ dmesg | tail
[2387259.786532] Goodbye
