
#ifdef __cplusplus
extern "C" {
#endif 

extern int _kbhit();
extern int _str_to_long(char *str, long* p_val, unsigned char base);
extern int _get_yes_no();
extern int _get_number(long int* pval);
extern int _get_physical_addr(long int* pval);
extern int _get_logical_addr(long int* pval);
extern int _get_power_status(long int* pval);
extern int fgets_ex(char* buf, int len);

#ifdef __cplusplus
}
#endif 
