#pragma once


// базовый менеджер графических ресурсов
class C3DBaseManager
{
public:

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*) = 0;

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*) = 0;
};
