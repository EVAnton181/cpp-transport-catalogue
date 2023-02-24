/*!
 * @file graph.h
 * @brief Заголовочный файл реализующий взвешенный ориентированный граф
 * 
 * @author Practicum
 * 
 * @version 1.0
 * @date Июнь 2022
 * 
*/
#pragma once

#include "ranges.h"
#include "domain.h"

#include <cstdlib>
#include <vector>

namespace graph {

using VertexId = size_t;
using EdgeId = size_t;

/// Структура с описанием ребра графа
template <typename Weight>
struct Edge {
    VertexId from;                                            ///< Вершина начала
    VertexId to;                                              ///< Вершина конца
    Weight weight;                                            ///< Вес ребра
    int stops_count;                                          ///< Количество остановок между вершинами
    domain::Bus* bus;                                         ///< Указатель на маршрут которому соответствует данное ребро
};

/*!
 * Класс реализующий направленный взвешенный граф
 */
template <typename Weight>
class DirectedWeightedGraph {
private:
    using IncidenceList = std::vector<EdgeId>; ///< Список номеров всех ребер
    using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

public:
	/// Конструктор по умолчанию
    DirectedWeightedGraph() = default;
	
	/// Конструктор от количества вершин (остановок)
    explicit DirectedWeightedGraph(size_t vertex_count);
	
	/// Конструктор от параметров
	DirectedWeightedGraph(std::vector<IncidenceList> incidence_lists, std::vector<Edge<Weight>> edges);
    
	/// Добавление ребра в граф, возвращает номер ребра
	EdgeId AddEdge(const Edge<Weight>& edge);

	/// Получить количество вершин в графе
    size_t GetVertexCount() const;
	
	/// Получить количество ребер в графе
    size_t GetEdgeCount() const;
	
	/// Получить ребро графа по номеру ребра
    const Edge<Weight>& GetEdge(EdgeId edge_id) const;
	
	/// Получить номера ребер выходящих из вершины vertex
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

private:
	std::vector<Edge<Weight>> edges_;   ///< Вектор ребер графа
    std::vector<IncidenceList> incidence_lists_; ///< Вектор векторов номеров графа от исходящей вершины
};

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
    : incidence_lists_(vertex_count) {
}

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(std::vector<IncidenceList> incidence_lists, std::vector<Edge<Weight>> edges)
	: edges_(edges)
	, incidence_lists_(incidence_lists) {
}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_.at(edge.from).push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
    return incidence_lists_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
    return edges_.size();
}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const {
    return edges_.at(edge_id);
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
    return ranges::AsRange(incidence_lists_.at(vertex));
}
}  // namespace graph
