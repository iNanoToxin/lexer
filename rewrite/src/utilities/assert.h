#ifndef ASSERT_H
#define ASSERT_H

#define LL_assert(p_Success, ...) assert_condition(__LINE__, __FILE__, #p_Success, p_Success, __VA_ARGS__)
#define LL_failure(...) failure(__LINE__, __FILE__, __VA_ARGS__)

void assert_condition(int p_Line, const char* p_File, const char* p_Condition, bool p_Success, const char* p_Message, const char* p_Identifier = nullptr);
void failure(int p_Line, const char* p_File, const char* p_Condition, const char* p_Message, const char* p_Identifier = nullptr);

#endif //ASSERT_H
