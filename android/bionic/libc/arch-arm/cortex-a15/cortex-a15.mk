libc_bionic_src_files_arm += \
    arch-arm/cortex-a15/bionic/stpcpy.S \
    arch-arm/cortex-a15/bionic/strcat.S \
    arch-arm/cortex-a15/bionic/__strcat_chk.S \
    arch-arm/cortex-a15/bionic/strcpy.S \
    arch-arm/cortex-a15/bionic/__strcpy_chk.S \
    arch-arm/cortex-a15/bionic/strlen.S \

libc_bionic_src_files_arm += \
    arch-arm/generic/bionic/memcmp.S \

libc_bionic_src_files_arm += arch-arm/cortex-a15/bionic/memcpy.S #326.94 1290.47 2414.06 2347.88 1911.59
#libc_bionic_src_files_arm += arch-arm/denver/bionic/memcpy.S #308.70 1450.69 2899.34 3043.26 2369.51 2331.18
#libc_bionic_src_files_arm += arch-arm/krait/bionic/memcpy.S #316.88 1264.27 2465.33 2502.10 1908.85

#libc_bionic_src_files_arm += arch-arm/generic/bionic/strcmp.S , bad bad
libc_bionic_src_files_arm += arch-arm/cortex-a15/bionic/strcmp.S 
#libc_bionic_src_files_arm += arch-arm/krait/bionic/strcmp.S  , bad

# libc_bionic_src_files_arm += arch-arm/cortex-a15/bionic/memset.S 362.26 1503.46 3998.45 3960.41 2667.61
# libc_bionic_src_files_arm += arch-arm/krait/bionic/memset.S 320.93 1790.08 4199.14 4526.33 2942.27
libc_bionic_src_files_arm += arch-arm/denver/bionic/memset.S #371.68 1934.65 4240.03 4320.73 3143.42

libc_bionic_src_files_arm += arch-arm/denver/bionic/memmove.S
#libc_bionic_src_files_arm += arch-arm/krait/bionic/memmove.S
