#pragma once


template<typename Type>
class List {
public:
	Type 		elem;

	List<Type>* next = nullptr;

	List(const Type & elem) {
		this->elem = elem;
	};

	~List() {
		if( next )
			delete(next);
		next = nullptr;
	}

	int Length()
	{
		if (nullptr == next) 
			return 1;
		else 
			return 1+next->Length();
	}

	//ajoute elem à la fin de liste
	void AddLast(const Type & elem) {
		if (next == nullptr)
			next = new List(elem); 
		else
			next->AddLast(elem);
	}

	//ajoute elem au début de la liste
	List<Type>* AddFirst(const Type & elem) {
		// auto toto
		// toto = [3 5 5]
		// toto = toto->Concat(4);
		// toto 4 [3 5 5 ]
		// toto [4 3 5 5 ]

		//créer un nouvel element de liste
		//accrocher soit meme au nouvel element
		//renvoyer le nouvel element

		auto Toto = new List(elem);
		Toto->next = this;
		return Toto;
	}

	List<Type>* Remove(const Type & removeme) {
		if (this->elem == removeme)
		{
			auto suitedeliste = next;
			this->next = nullptr;
			delete this;
			if (suitedeliste != nullptr )
				return suitedeliste->Remove(removeme);
			else
				return nullptr;
		}
		else
		{
			if (next)
				next = next->Remove(removeme);
			return this;
		}
	}

	bool Contains(const Type & elem) {

	}
};

typedef List<int> IntList;
