/*
 * (C) Copyright 2002-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * To build the utility with the run-time configuration
 * uncomment the next line.
 * See included "fw_env.config" sample file (TRAB board)
 * for notes on configuration.
 */
//#define CONFIG_FILE     "/etc/fw_env.config"

//#define EXIT_SUCCESS	0
//#define EXIT_FAILURE -1

#define HAVE_REDUND /* For systems with 2 env sectors */
#define DEVICE1_NAME      "/dev/mtd1"
#define DEVICE2_NAME      "/dev/mtd2"
#define DEVICE1_OFFSET    0x0000
#define ENV1_SIZE         0x4000
#define DEVICE1_ESIZE     0x4000
#define DEVICE2_OFFSET    0x0000
#define ENV2_SIZE         0x4000
#define DEVICE2_ESIZE     0x4000

#define CONFIG_BAUDRATE		115200
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#define CONFIG_BOOTCOMMAND							\
	"bootp; "								\
	"setenv bootargs root=/dev/nfs nfsroot=${serverip}:${rootpath} "	\
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off; "	\
	"bootm"

#define DBG(...) //printf("%s#%d: ", __FUNCTION__, __LINE__); printf(__VA_ARGS__);
#define ERR(...) printf("ERR %s#%d: ", __FUNCTION__, __LINE__); printf(__VA_ARGS__);


typedef unsigned long ulong;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;

extern int   fw_printenv(int argc, char *argv[]);
extern char *fw_getenv  (const char *name);
extern int fw_getenv_i (const char *name);

//extern int fw_setenv  (int argc, char *argv[]);
extern int fw_setenv(const char *name,  char *value);

extern int fw_parse_script(char *fname);
extern int fw_env_open(void);
extern int fw_env_write(const char *name, char *value);
extern int fw_env_close(void);

extern uint32_t crc32(uint32_t crc, unsigned char const *p, size_t len);
//unsigned	long  crc32	 (unsigned long, const unsigned char *, unsigned);
