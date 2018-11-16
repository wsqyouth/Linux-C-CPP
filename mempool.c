#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
参考网站：https://www.cnblogs.com/dchipnau/p/5255277.html
环境：vs2013 __FUNCTION__  __FUNC__
mempool维护了两个链表
建立链表：凡是create的chunk都是用createnext连接，初始是标志位为1（表示已使用)
free链表：chunk都是用next连接，都是未使用。

get时从free链表头删，release时从free链表头插（同时更新当前可用chunk数，每个chunk的使用标志)
destory时要记得两次释放资源
当内存池不够使用时，重新create
*/
typedef struct _mem_chunk {
	void* data;
	int uesdflag;//是否已使用
	struct _mem_chunk *next;//空闲链表连接
	struct _mem_chunk *createnext; //所有建立的结点都在该链表上
} mem_chunk;

typedef struct _mempool {
	int mp_free_chunks; //可用单元格个数
	int mp_total_chunks;//总的单元格个数
	int mp_chunk_size; //单元格大小

	mem_chunk *mp_createlinkhead;//创建的memblock链表头，内存释放时使用
	mem_chunk *mp_freelinkhead;//空闲memblock链表头
} mempool;

mem_chunk * mempool_allocchunk(mempool *mp) {
	mem_chunk *chunk = (mem_chunk *)malloc(sizeof(mem_chunk));
	chunk->data = malloc(sizeof(mp->mp_chunk_size));
	chunk->next = NULL;
	chunk->uesdflag = 1; //表示已使用

	//加入创建的链表(头插)
	chunk->createnext = mp->mp_createlinkhead;
	mp->mp_createlinkhead = chunk;

	mp->mp_total_chunks++;

	return (chunk);
}

void mempool_release(mempool *mp, mem_chunk *chunk) {
	if (chunk == NULL) {
		printf("%s:release a null chunk", __FUNCTION__);
		return;
	}
	if (chunk->uesdflag != 1){
		printf("%s:used!=1", __FUNCTION__);
		return;
	}

	//归还内存块放到空闲链表头
	chunk->uesdflag = 0; //表示空闲
	chunk->next = mp->mp_freelinkhead;
	mp->mp_freelinkhead = chunk;

	mp->mp_free_chunks++;
}

void* mempool_get(mempool *mp) {
	mem_chunk *chunk = NULL;
	if (mp->mp_freelinkhead != NULL) {
		//从空闲链表头取出一个
		chunk = mp->mp_freelinkhead;
		mp->mp_freelinkhead = mp->mp_freelinkhead->next;
		chunk->next = NULL;
		chunk->uesdflag = 1; //表示已使用

		mp->mp_free_chunks--;//空闲内存块数-1
	}
	else {
		//没有空闲内存块，创建一个
		chunk = mempool_allocchunk(mp);
	}

	return (chunk);
}

//每个chunk大小为chunk_size，共计total_size个
mempool *mempool_create(int chunk_size, size_t total_size) {
	if (chunk_size < (int)sizeof(mem_chunk)) {
		printf("too small --> chunk_size: %d\n", chunk_size);
		return NULL;
	}
		
	if (chunk_size % 4 != 0) {
		printf("mempool_create --> chunk_size: %d\n", chunk_size);
		return NULL;
	}

	mempool *mp = (mempool *)malloc(sizeof(mempool));
	if (mp == NULL) {
		printf("mempool_create --> malloc failed\n");
		return NULL;
	}

	mp->mp_chunk_size = chunk_size; 
	mp->mp_free_chunks = 0;
	mp->mp_total_chunks = 0;

	mp->mp_freelinkhead = NULL;
	//预先初始化total_size个内存块
	mp->mp_createlinkhead = NULL;
	for (size_t i = 0; i < total_size; ++i) {
		mem_chunk *chunk = mempool_allocchunk(mp);
		mempool_release(mp, chunk);
	}
	printf("create %s:total size(%d),unused(%d)\n", __FUNCTION__, mp->mp_total_chunks, mp->mp_free_chunks);
	return (mp);
}

void mempool_destory(mempool *mp) {
	//printf("destroy %s:total size(%d),unused(%d)\n", __FUNCTION__, mp->mp_total_chunks, mp->mp_free_chunks);
	mem_chunk *chunk = NULL;
	//将所有创建的chunk释放掉
	while (mp->mp_createlinkhead !=NULL)
	{
		chunk = mp->mp_createlinkhead;
		mp->mp_createlinkhead = mp->mp_createlinkhead->createnext;
		if (chunk->data)
			free(chunk->data);
		if (chunk)
			free(chunk);
	}
	free(mp);
	
}


int main()
{
	printf("sizeof(mem_chunk):%d\n", sizeof(mem_chunk));
	mempool *mp = mempool_create(24, 3);
	mem_chunk *chunk = (mem_chunk *)mempool_get(mp);
	printf("%p\n", chunk);
	mem_chunk *chunk1 = (mem_chunk *)mempool_get(mp);
	printf("%p\n", chunk1);
	mem_chunk *chunk2 = (mem_chunk *)mempool_get(mp);
	printf("%p\n", chunk2);
	printf(" %s:total size(%d),unused(%d)\n", __FUNCTION__, mp->mp_total_chunks, mp->mp_free_chunks);
	mempool_release(mp,chunk1);
	mempool_release(mp, chunk2);
	printf(" %s:total size(%d),unused(%d)\n", __FUNCTION__, mp->mp_total_chunks, mp->mp_free_chunks);
	mem_chunk *chunk3 = (mem_chunk *)mempool_get(mp);
	printf("%p\n", chunk3);
	mem_chunk *chunk4 = (mem_chunk *)mempool_get(mp);
	printf("%p\n", chunk4);
	printf("%s:total size(%d),unused(%d)\n", __FUNCTION__, mp->mp_total_chunks, mp->mp_free_chunks);
	printf("chunk3 chunk4使用的是已有内存池中的额\n");
	mempool_destory(mp);
	printf("hello\n");
	return 0;
}
