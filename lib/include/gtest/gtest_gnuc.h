
//解决mstar ecos编译问题
#ifndef __GTEST_GNUC_H__
#define __GTEST_GNUC_H__

#if defined(CFG_CHSSA_CHIP_MSTAR5C69)||defined(CFG_CHSSA_CHIP_MSTAR5C58)||defined(CFG_CHSSA_CHIP_MSTAR7C123D)
#ifdef __GNUC__
#undef __GNUC__
#endif
#define __GNUC__ 2
#endif /* DCFG_CHSSA_CHIP_MSTAR5C58 */

#endif

