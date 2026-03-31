from sieve_of_eratosthenes_binding import sieve_of_eratosthenes

a= (sieve_of_eratosthenes(1050623))
for i in range(len(a)):
    if a[i]:
        print(i)
