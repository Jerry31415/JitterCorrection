#pragma once
#include <vector>
#include <algorithm>
#include <iostream>

// Находит локльный минимум функции S от N аргументов
template<typename T>
class GDM{
	std::vector<T> G;
	std::vector<T> t;

	T dist(const std::vector<T>& v){
		T res = 0;
		for (int i = 0; i < v.size(); ++i) res += v[i] * v[i];
		return sqrt(res);
	}

public:

	virtual ~GDM(){
	}

	void init(int N){
		G.resize(N, 0);
		t.resize(N, 0);
	}

	// Функция S должна быть определена пользователем
	virtual T S(std::vector<T>& arg) = 0;

	// Возвращает численную аппроксимацию первой 
	// частной производной функции S по аргументу с номером k
	T dS_arg_k(std::vector<T>& arg, int k, T eps = 0.001){
		if (k < 0 || k >= t.size()){
			throw std::runtime_error("Error in dS_arg_k function: incorrect k argument value\n");
		}
		std::copy(arg.begin(), arg.end(), t.begin());
		T S0, S1;
		t[k] -= eps;
		S0 = S(t);
		t[k] += 2 * eps;
		S1 = S(t);
		return (S1 - S0) / (2.f*eps);
	}

	// Возвращает численную аппроксимацию первой 
	// частной производной функции S по аргументу с номером k
	T dS_arg_kO(std::vector<T>& arg, int k, T SAarg){
		T S0;
		arg[k] -= 1;
		S0 = S(arg);
		arg[k] += 1;
		return (SAarg - S0);
	}

	// Возвращет в dst вектор-градиент функции S(arg)
	void gradient(std::vector<T>& arg, std::vector<T>& dst, T eps = 0.001){
		if (dst.empty()) dst.resize(arg.size());
		for (int k = 0; k < arg.size(); ++k){
			dst[k] = dS_arg_k(arg, k, eps);
		}
	}

	void step(std::vector<T>& dst, T learning_rate, T eps = 1.){
		if (dst.empty()) {
			throw std::runtime_error("Error in solve function: dst is empty\n");
		}
		if (t.empty()){
			init(dst.size());
		}
		for (int z = 0; z < t.size(); ++z) t[z] = 0;
		gradient(dst, G, eps);
		T GD = dist(G);
		if (GD < 0.000001) return;
		for (auto& e : G) e /= GD;
		for (int z = 0; z < dst.size(); ++z) dst[z] -= learning_rate*G[z];
	}

	void step(std::vector<T>& dst, T learning_rate, std::vector<bool>& trainable_flags, T eps = 1.){
		if (dst.empty()) {
			throw std::runtime_error("Error in solve function: dst is empty\n");
		}
		if (t.empty()){
			init(dst.size());
		}
		for (int z = 0; z < t.size(); ++z) t[z] = 0;
		gradient(dst, G, eps);
		T GD = dist(G);
		if (GD < 0.000001) return;
		for (auto& e : G) e /= GD;
		for (int z = 0; z < dst.size(); ++z) 
			dst[z] -= trainable_flags[z] * learning_rate*G[z];
	}

#ifdef WITH_SOLVER
	// Производит itter иттераций метода Ньютона для минимизации S.
	// Результат возвращает в dst.
	// Если useApproximation = true, то использует значение dst в качестве начального приближения 
	void solve(int itter, std::vector<T>& dst, bool useApproximation = false, T eps = 1){
		if (dst.empty()) {
			if (useApproximation) throw std::runtime_error("Error in solve function: dst is empty\n");
			dst.resize(t.size(), 0);
		}
		int cnt = 0;
		for (int z = 0; z < t.size(); ++z) t[z] = 0;
		T S0, S1;
		bool shake = false;
		do{
			S0 = S(dst);
			if (!shake)	gradient(dst, G, eps);
			T GD = dist(G);
			if (GD < 0.00005) {
				break;
			}
			for (auto& e : G) e /= GD;
			T lambda = 0;
			
			std::vector<T> tmp(dst.size(), 0);
			for (int z = 0; z < dst.size(); ++z) tmp[z] = dst[z] + G[z];
			T SvalM1 = S(tmp);
			for (int z = 0; z < dst.size(); ++z) tmp[z] = dst[z] - G[z];
			T SvalP1 = S(tmp);
			for (int z = 0; z < dst.size(); ++z) tmp[z] = dst[z];
			T Sval = S(tmp);
			if (Sval>SvalM1 && Sval>SvalP1){
				if (SvalM1 < SvalP1) lambda = -1;
				else lambda = 1;
			}
			else {
				T den = 2 * (2 * Sval - SvalM1 - SvalP1);
				if (abs(den) < 0.000001){
					if (Sval <= SvalM1 && Sval <= SvalP1) lambda = 0;
					else if (SvalM1 <= Sval && SvalM1 <= SvalP1) lambda = -1;
					else if (SvalP1 <= Sval && SvalP1 <= SvalM1) lambda = 1;
					else {
						std::cout << "Error\n";
					}
				}
				else lambda = (SvalP1 - SvalM1) / den;
			}
			for (int z = 0; z < dst.size(); ++z) dst[z] = dst[z] - lambda*G[z];
			S1 = S(dst);
			++cnt;
		} while (cnt<itter);
	}
#endif
};