/*
 * os_cfg.h
 *
 *  Created on: 10 févr. 2016
 *      Author: reiboul
 */

#ifndef SRC_OS_CFG_OS_CFG_H_
#define SRC_OS_CFG_OS_CFG_H_

/* ALl supported targets */
#define LINUX_x86 1
#define ARDUINO   2

#ifndef TARGET
#error You must define a target
#endif


#endif /* SRC_OS_CFG_OS_CFG_H_ */
