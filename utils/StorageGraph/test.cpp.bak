#include <iostream>
using std::cout; using std::cin; using std::endl;

#include <string>
using std::string;

#include <stdexcept>

#include "./StorageGraph.h"


int main(int argc, char **argv) {

  string sample[] = {
    "Nov 13 07:46:53 localhost bootlog[0]: BOOT_TIME 1510530413 0",
    "Nov 13 07:47:59 MacBook-Air loginwindow[76]: USER_PROCESS: 76 console",
    "Nov 13 07:48:02 MacBook-Air loginwindow[76]: DEAD_PROCESS: 76 console",
    "Nov 13 07:48:02 MacBook-Air loginwindow[76]: USER_PROCESS: 76 console",
    "Nov 13 07:52:16 zhuxiaoguangs-MacBook-Air loginwindow[505]: USER_PROCESS: 505 console",
    "Nov 13 07:52:44 zhuxiaoguangs-MacBook-Air sessionlogoutd[720]: DEAD_PROCESS: 76 console",
    "Nov 13 07:59:22 zhuxiaoguangs-MacBook-Air login[865]: USER_PROCESS: 865 ttys000",
    "Nov 13 08:01:15 zhuxiaoguangs-MacBook-Air login[865]: DEAD_PROCESS: 865 ttys000",
    "Nov 13 08:01:35 zhuxiaoguangs-MacBook-Air login[899]: USER_PROCESS: 899 ttys000",
    "Nov 13 08:01:36 zhuxiaoguangs-MacBook-Air login[899]: DEAD_PROCESS: 899 ttys000",
    "Nov 13 08:01:38 zhuxiaoguangs-MacBook-Air login[917]: USER_PROCESS: 917 ttys000",
    "Nov 13 08:01:44 zhuxiaoguangs-MacBook-Air login[917]: DEAD_PROCESS: 917 ttys000",
    "Nov 13 08:06:18 zhuxiaoguangs-MacBook-Air login[1046]: USER_PROCESS: 1046 ttys000",
    "Nov 13 08:06:31 zhuxiaoguangs-MacBook-Air login[1046]: DEAD_PROCESS: 1046 ttys000",
    "Nov 13 08:20:06 zhuxiaoguangs-MacBook-Air login[1165]: USER_PROCESS: 1165 ttys000",
    "Nov 13 08:20:13 zhuxiaoguangs-MacBook-Air login[1165]: DEAD_PROCESS: 1165 ttys000",
    "Nov 13 08:22:03 zhuxiaoguangs-MacBook-Air login[1203]: USER_PROCESS: 1203 ttys000",
    "Nov 13 08:30:14 zhuxiaoguangs-MacBook-Air login[1203]: DEAD_PROCESS: 1203 ttys000",
    "Nov 13 08:40:48 zhuxiaoguangs-MacBook-Air login[1453]: USER_PROCESS: 1453 ttys000",
    "Nov 13 08:41:15 zhuxiaoguangs-MacBook-Air login[1453]: DEAD_PROCESS: 1453 ttys000",
    "Nov 13 08:42:15 zhuxiaoguangs-MacBook-Air login[1498]: USER_PROCESS: 1498 ttys000",
    "Nov 13 08:42:21 zhuxiaoguangs-MacBook-Air login[1498]: DEAD_PROCESS: 1498 ttys000",
    "Nov 13 08:50:24 zhuxiaoguangs-MacBook-Air login[1633]: USER_PROCESS: 1633 ttys000",
    "Nov 13 08:50:37 zhuxiaoguangs-MacBook-Air login[1633]: DEAD_PROCESS: 1633 ttys000",
    "Nov 13 08:51:29 zhuxiaoguangs-MacBook-Air login[1664]: USER_PROCESS: 1664 ttys000",
    "Nov 13 08:53:07 zhuxiaoguangs-MacBook-Air login[1664]: DEAD_PROCESS: 1664 ttys000",
    "Nov 13 08:55:06 zhuxiaoguangs-MacBook-Air sessionlogoutd[1879]: DEAD_PROCESS: 505 console",
    "Nov 13 08:55:09 zhuxiaoguangs-MacBook-Air shutdown[1899]: SHUTDOWN_TIME: 1510534509 944501",
    "Nov 13 08:56:36 localhost bootlog[0]: BOOT_TIME 1510534596 0",
    "Nov 13 09:10:33 localhost bootlog[0]: BOOT_TIME 1510535433 0",
    "Nov 13 09:20:31 localhost bootlog[0]: BOOT_TIME 1510536031 0",
    "Nov 13 09:20:57 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 09:22:03 zhuxiaoguangs-MacBook-Air sessionlogoutd[369]: DEAD_PROCESS: 87 console",
    "Nov 13 09:22:03 zhuxiaoguangs-MacBook-Air shutdown[370]: SHUTDOWN_TIME: 1510536123 823873",
    "Nov 13 09:56:46 localhost bootlog[0]: BOOT_TIME 1510538206 0",
    "Nov 13 09:57:08 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 09:57:59 zhuxiaoguangs-MacBook-Air login[367]: USER_PROCESS: 367 ttys000",
    "Nov 13 10:02:27 localhost bootlog[0]: BOOT_TIME 1510538547 0",
    "Nov 13 10:02:50 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 10:02:51 zhuxiaoguangs-MacBook-Air login[275]: USER_PROCESS: 275 ttys000",
    "Nov 13 10:04:56 zhuxiaoguangs-MacBook-Air login[275]: DEAD_PROCESS: 275 ttys000",
    "Nov 13 10:07:49 localhost bootlog[0]: BOOT_TIME 1510538869 0",
    "Nov 13 10:08:12 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 10:08:54 zhuxiaoguangs-MacBook-Air login[371]: USER_PROCESS: 371 ttys000",
    "Nov 13 10:10:31 zhuxiaoguangs-MacBook-Air login[371]: DEAD_PROCESS: 371 ttys000",
    "Nov 13 10:10:50 zhuxiaoguangs-MacBook-Air sessionlogoutd[428]: DEAD_PROCESS: 87 console",
    "Nov 13 10:10:50 zhuxiaoguangs-MacBook-Air shutdown[429]: SHUTDOWN_TIME: 1510539050 969371",
    "Nov 13 10:59:32 localhost bootlog[0]: BOOT_TIME 1510541972 0",
    "Nov 13 10:59:54 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 11:02:48 zhuxiaoguangs-MacBook-Air sessionlogoutd[501]: DEAD_PROCESS: 87 console",
    "Nov 13 11:02:48 zhuxiaoguangs-MacBook-Air shutdown[502]: SHUTDOWN_TIME: 1510542168 239548",
    "Nov 13 11:03:12 localhost bootlog[0]: BOOT_TIME 1510542192 0",
    "Nov 13 11:03:35 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 11:04:34 zhuxiaouangsAir login[406]: USER_PROCESS: 406 ttys000",
    "Nov 13 11:05:04 zhuxiaouangsAir login[406]: DEAD_PROCESS: 406 ttys000",
    "Nov 13 11:05:52 zhuxiaoguangs-MacBook-Air sessionlogoutd[489]: DEAD_PROCESS: 87 console",
    "Nov 13 11:05:52 zhuxiaoguangs-MacBook-Air shutdown[490]: SHUTDOWN_TIME: 1510542352 314589",
    "Nov 13 12:25:56 localhost bootlog[0]: BOOT_TIME 1510547156 0",
    "Nov 13 12:26:23 zhuxiaoguangs-MacBook-Air loginwindow[87]: USER_PROCESS: 87 console",
    "Nov 13 12:32:18 zhuxiaoguangs-MacBook-Air login[557]: USER_PROCESS: 557 ttys000",
    "Nov 13 12:32:59 zhuxiaoguangs-MacBook-Air login[557]: DEAD_PROCESS: 557 ttys000",
    "Nov 13 12:45:00 zhuxiaoguangs-MacBook-Air login[911]: USER_PROCESS: 911 ttys000",
    "Nov 13 12:45:19 zhuxiaoguangs-MacBook-Air login[911]: DEAD_PROCESS: 911 ttys000",
    "Nov 13 12:45:21 zhuxiaoguangs-MacBook-Air login[919]: USER_PROCESS: 919 ttys000",
    "Nov 13 12:45:38 zhuxiaoguangs-MacBook-Air login[919]: DEAD_PROCESS: 919 ttys000",
    "Nov 13 12:45:39 zhuxiaoguangs-MacBook-Air login[940]: USER_PROCESS: 940 ttys000",
    "Nov 13 12:45:55 zhuxiaoguangs-MacBook-Air login[940]: DEAD_PROCESS: 940 ttys000",
    "Nov 13 12:45:57 zhuxiaoguangs-MacBook-Air login[949]: USER_PROCESS: 949 ttys000",
    "Nov 13 12:46:14 zhuxiaoguangs-MacBook-Air login[949]: DEAD_PROCESS: 949 ttys000",
    "Nov 13 12:47:41 zhuxiaoguangs-MacBook-Air login[995]: USER_PROCESS: 995 ttys000",
    "Nov 13 12:48:39 zhuxiaoguangs-MacBook-Air login[1011]: USER_PROCESS: 1011 ttys001",
    "Nov 13 13:06:28 zhuxiaoguangs-MacBook-Air login[1011]: DEAD_PROCESS: 1011 ttys001",
    "Nov 13 13:06:29 zhuxiaoguangs-MacBook-Air login[995]: DEAD_PROCESS: 995 ttys000",
    "Nov 13 13:32:51 zhuxiaoguangs-MacBook-Air login[4864]: USER_PROCESS: 4864 ttys000",
    "Nov 13 13:33:06 zhuxiaoguangs-MacBook-Air login[4864]: DEAD_PROCESS: 4864 ttys000",
    "Nov 13 13:37:43 zhuxiaoguangs-MacBook-Air login[4909]: USER_PROCESS: 4909 ttys000",
    "Nov 13 13:37:48 zhuxiaoguangs-MacBook-Air login[4909]: DEAD_PROCESS: 4909 ttys000",
    "Nov 13 13:46:33 zhuxiaoguangs-MacBook-Air login[5439]: USER_PROCESS: 5439 ttys000",
    "Nov 13 13:46:44 zhuxiaoguangs-MacBook-Air login[5439]: DEAD_PROCESS: 5439 ttys000",
    "Nov 13 13:48:10 zhuxiaoguangs-MacBook-Air login[5471]: USER_PROCESS: 5471 ttys000",
    "Nov 13 14:17:34 zhuxiaoguangs-MacBook-Air login[5471]: DEAD_PROCESS: 5471 ttys000"
  };

  size_t MESSAGE_TABLE_SIZE = 5000;
  auto storage = Storage(MESSAGE_TABLE_SIZE, 200);

  auto filename = string("../../test_env/modified_syslog.txt");

  cout << "Reading from file \"" << filename << "\"" << endl;

  storage.read_from_file(filename);
  cout << "Storage::read_from_file() passed!" << endl;
  /* cout << "Press <Enter> to reveal stats\n"; getchar(); */

  size_t inplace_cnt = 0;
  for (size_t idx = 0; idx != MESSAGE_TABLE_SIZE; ++idx) {
    if ((*storage.messages)[idx].occupied()) { ++inplace_cnt; }
  }
  cout << "MESSAGE_TABLE_SIZE ==> " << MESSAGE_TABLE_SIZE << endl;
  cout << "inplace_cnt ==> " << inplace_cnt << endl;
  cout << "Sp. Eff. ==> "
       << static_cast<double>(inplace_cnt)
          / static_cast<double>(MESSAGE_TABLE_SIZE)
          * 100.0
       << "%" << endl;

  cout << "End of Test!" << endl;

  return 0;

}
