
cdef extern from "sieve_of_eratosthenes.h":
    ctypedef bint bool
    cdef bool *SieveOfEratosthenes(int n)
    cdef int * Convert(bool *a)


def sieve_of_eratosthenes(int n):
    cdef bool* sieve =SieveOfEratosthenes(n)
    result= [False]*n
    for i in range(n):
        if(sieve[i]==True):
            result[i]=True
    return result

