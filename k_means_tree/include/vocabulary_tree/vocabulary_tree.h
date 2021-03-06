#ifndef VOCABULARY_TREE_H
#define VOCABULARY_TREE_H

#include "k_means_tree/k_means_tree.h"
#include <cereal/types/map.hpp>

/*
 * k_means_tree
 *
 * This class describes a collection of points that occurr together
 *
 */

struct inverted_file {
    std::map<int, int> source_id_freqs; // change this to uint32_t, uint32_t
    template <class Archive> void serialize(Archive& archive)
    {
        archive(source_id_freqs);
    }
};

template <typename Point, size_t K>
class vocabulary_tree : public k_means_tree<Point, K, inverted_file> {
protected:

    using super = k_means_tree<Point, K, inverted_file>;
    using typename super::PointT;
    using typename super::CloudT;
    using typename super::CloudPtrT;

    static const bool classic_pyramid_match = true;
    bool do_print;

public:

    using cloud_idx_score = std::pair<int, double>;
    using typename super::leaf;
    using typename super::node;

protected:

    std::vector<int> indices; // the source indices of the points (image ids of features), change this to uint32_t
    std::map<int, double> db_vector_normalizing_constants; // normalizing constants for the p vectors
    double N; // number of sources (images) in database
    static const bool normalized = true;
    int matching_min_depth;

protected:

    double pexp(const double v) const;
    double proot(const double v) const;
    double compute_query_vector(std::map<node*, double>& query_id_freqs, CloudPtrT& query_cloud);
    void compute_query_vector(std::map<node*, int>& query_id_freqs, CloudPtrT& query_cloud);
    double compute_query_vector(std::map<node*, std::pair<double, int> >& query_id_freqs, CloudPtrT& query_cloud);
    void source_freqs_for_node(std::map<int, int>& source_id_freqs, node* n) const;
    size_t nbr_sources_for_freqs(std::map<int, int>& source_id_freqs) const;
    void normalizing_constants_for_node(std::map<int, int>& normalizing_constants, node* n, int current_depth);

    void compare_vocabulary_vectors(std::map<node*, double>& query_id_freqs);

    void unfold_nodes(std::vector<node*>& path, node* n, const PointT& p, std::map<node*, double>& active);
    void get_path_for_point(std::vector<node*>& path, const PointT& point, std::map<node*, double>& active);
    void compute_vocabulary_vector(std::map<node*, double>& query_id_freqs,
                                   CloudPtrT& query_cloud, std::map<node*, double>& active);

public:

    double compute_new_weights(std::map<int, double>& original_norm_constants, std::map<node*, double>& original_weights,
                               std::map<int, double>& weighted_indices, CloudPtrT& query_cloud);
    double compute_new_weights(std::map<int, double>& original_norm_constants, std::map<node*, double>& original_weights,
                               std::vector<double>& weights, std::vector<CloudPtrT>& clouds, CloudPtrT& query_cloud);
    double restore_old_weights(std::map<int, double>& original_norm_constants, std::map<node*, double>& original_weights);

    double compute_vocabulary_norm(CloudPtrT& cloud);
    double compute_min_combined_dist(std::vector<int>& smallest_ind_combination, CloudPtrT& cloud, std::vector<CloudPtrT>& smaller_clouds,
                                     std::vector<double>& pnorms, pcl::PointCloud<pcl::PointXYZRGB>::Ptr& centers);
    double compute_min_combined_dist(std::vector<int>& smallest_ind_combination, CloudPtrT& cloud, std::vector<std::map<int, double> >& smaller_freqs, std::vector<double>& pnorms,
                                     pcl::PointCloud<pcl::PointXYZRGB>::Ptr& centers, std::map<node*, int>& mapping, int hint = -1);
    double compute_min_combined_dist(std::vector<int>& smallest_ind_combination, CloudPtrT& cloud, std::vector<std::map<int, int> >& smaller_freqs,
                                     pcl::PointCloud<pcl::PointXYZRGB>::Ptr& centers, std::map<node*, int>& mapping, std::map<int, node *>& inverse_mapping, int hint = -1);

    void set_min_match_depth(int depth);
    void compute_normalizing_constants(); // this also computes the weights
    bool empty() const { return indices.empty(); }
    int max_ind() const { return *(std::max_element(indices.begin(), indices.end())) + 1; }
    void set_input_cloud(CloudPtrT& new_cloud, std::vector<int>& new_indices);
    void append_cloud(CloudPtrT& extra_cloud, std::vector<int>& extra_indices, bool store_points = true);
    void add_points_from_input_cloud(bool save_cloud = true);
    void top_similarities(std::vector<cloud_idx_score>& scores, CloudPtrT& query_cloud, size_t nbr_results = 20);

    void top_larger_similarities(std::vector<cloud_idx_score>& scores, CloudPtrT& query_cloud, size_t nbr_results);
    void top_smaller_similarities(std::vector<cloud_idx_score>& scores, CloudPtrT& query_cloud, size_t nbr_results);
    void top_combined_similarities(std::vector<cloud_idx_score>& scores, CloudPtrT& query_cloud, size_t nbr_results);

    double compute_query_index_vector(std::map<int, double>& query_index_freqs, CloudPtrT& query_cloud, std::map<node*, int>& mapping);
    void compute_query_index_vector(std::map<int, int>& query_index_freqs, CloudPtrT& query_cloud, std::map<node*, int>& mapping);
    size_t deep_count_sets();

    template <class Archive> void save(Archive& archive) const;
    template <class Archive> void load(Archive& archive);

    vocabulary_tree() : super(5), matching_min_depth(1), do_print(false) {}

};

#include "vocabulary_tree.hpp"

#endif // VOCABULARY_TREE_H
