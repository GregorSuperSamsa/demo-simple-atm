# demo-simple-atm
Demo code example for a simple ATM server

# Filesystem is used as a persistent storage:
* *Files should reside in the executable directory* * \
**credentials** \
- stores user accounts in format username:password, **should be manually created** \
**transfers**
- stores pending and already closed transfers, **auto created at runtime** \
**user:password:history** \
- stores history for the user session, **auto created at runtime** \
**user:password:balance** \
- stores balance of the user account, **auto created at runtime**

# User access:
Remote user access is via TCP (port **8889**)
Local user access is via command line (uses TCP internally)

# Supported commands:
**login**     - Create new user sesssion: login username password \
**logout**    - Close current user session \
**balance**   - Check your current balance \
**deposit**   - Deposit amount, as a whole number only, to your account: deposit amount \
**withdraw**  - Withdraw amount, as a whole number only, from your account: withdraw amount \
**transfer**  - Transfer amount, as a whole number only, to another user account: transfer amount username (receiver) \
**history**   - Show list of previous actions \
**help**      - Show help
