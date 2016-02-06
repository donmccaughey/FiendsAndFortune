#ifndef FNF_DUNGEON_DUNGEON_GENERATOR_H_INCLUDED
#define FNF_DUNGEON_DUNGEON_GENERATOR_H_INCLUDED


struct digger;
struct dungeon;
struct rnd;


struct dungeon_generator {
    struct digger **diggers;
    int diggers_count;
    struct dungeon *dungeon;
    int iteration_count;
    int max_depth;
    int max_length;
    int max_width;
    struct rnd *rnd;
};


struct dungeon_generator *
dungeon_generator_alloc(struct dungeon *dungeon, struct rnd *rnd);

void
dungeon_generator_free(struct dungeon_generator *generator);

void
dungeon_generator_generate(struct dungeon_generator *generator);

void
dungeon_generator_generate_small(struct dungeon_generator *generator);

void
dungeon_generator_add_digger(struct dungeon_generator *generator,
                             struct digger *digger);

void
dungeon_generator_remove_digger(struct dungeon_generator *generator,
                                struct digger *digger);


#endif
