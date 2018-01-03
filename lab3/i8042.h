#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define ESC_KEY         0x81
#define SPC_KEY			0XE0

#define KBD_IRQ	        1

#define DELAY_US 		20000

#define STAT_REG        0x64
#define STATUS_PORT 	0x64
#define KBD_OUT_BUF	 	0x60
#define CMD_DATA 		0X20
#define KBD_CMD_BUF 	0x64

#define BIT(n) 			(0x01 << (n))
#define PARITY			BIT(7)
#define TIMEOUT 		BIT(6)
#define IBF 			BIT(1)
#define OBF				BIT(0)

#endif /* _LCOM_I8042_H */
