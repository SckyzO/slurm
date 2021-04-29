/*****************************************************************************\
 *  step_ctx.h - step context declarations
 *****************************************************************************
 *  Copyright (C) 2006 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Morris Jette <jette1@llnl.gov>,
 *  Christopher J. Morrone <morrone2@llnl.gov>
 *  CODE-OCEC-09-009. All rights reserved.
 *
 *  This file is part of Slurm, a resource management program.
 *  For details, see <https://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  Slurm is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  Slurm is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Slurm; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/
#ifndef _STEP_CTX_H
#define _STEP_CTX_H

#include <inttypes.h>
#include <unistd.h>

#include "slurm/slurm.h"

#include "src/api/step_launch.h"

#define STEP_CTX_MAGIC 0xc7a3

struct slurm_step_ctx_struct {
	uint16_t magic;	/* magic number */

	uint32_t job_id;	/* assigned job id */
	uint32_t user_id;	/* user the job runs as */

	job_step_create_request_msg_t *step_req;
	job_step_create_response_msg_t *step_resp;

	/* Used by slurm_step_launch() */
	struct step_launch_state *launch_state;
	uint16_t verbose_level; /* for extra logging decisions in step
				 * launch api */
};


/*
 * slurm_step_ctx_params_t_init - This initializes parameters
 *	in the structure that you will pass to slurm_step_ctx_create().
 *	This function will NOT allocate any new memory.
 * IN ptr - pointer to a structure allocated by the user.  The structure will
 *      be initialized.
 */
extern void slurm_step_ctx_params_t_init(slurm_step_ctx_params_t *ptr);

/*
 * slurm_step_ctx_create_timeout - Create a job step and its context.
 * IN step_params - job step parameters
 * IN timeout - in milliseconds
 * RET the step context or NULL on failure with slurm errno set
 * NOTE: Free allocated memory using slurm_step_ctx_destroy.
 */
extern slurm_step_ctx_t *slurm_step_ctx_create_timeout(const slurm_step_ctx_params_t *step_params,
						       int timeout);

/*
 * Return TRUE if the job step create request should be retried later
 * (i.e. the errno set by slurm_step_ctx_create_timeout() is recoverable).
 */
extern bool slurm_step_retry_errno(int rc);

/*
 * slurm_step_ctx_create_no_alloc - Create a job step and its context without
 *                                  getting an allocation.
 * IN step_params - job step parameters
 * IN step_id     - since we are faking it give me the id to use
 * RET the step context or NULL on failure with slurm errno set
 * NOTE: Free allocated memory using slurm_step_ctx_destroy.
 */
extern slurm_step_ctx_t *slurm_step_ctx_create_no_alloc(const slurm_step_ctx_params_t *step_params,
							uint32_t step_id);

/*
 * slurm_step_ctx_get - get parameters from a job step context.
 * IN ctx - job step context generated by slurm_step_ctx_create
 * RET SLURM_SUCCESS or SLURM_ERROR (with slurm_errno set)
 */
extern int slurm_step_ctx_get(slurm_step_ctx_t *ctx, int ctx_key, ...);

/*
 * slurm_jobinfo_ctx_get - get parameters from jobinfo context.
 * IN jobinfo - job information from context, returned by slurm_step_ctx_get()
 * IN data_type - type of data required, specific to the switch type
 * OUT data - the requested data type
 * RET SLURM_SUCCESS or SLURM_ERROR (with slurm_errno set)
 */
extern int slurm_jobinfo_ctx_get(dynamic_plugin_data_t *jobinfo,
				 int data_type,
				 void *data);

/*
 * slurm_step_ctx_daemon_per_node_hack - Hack the step context
 *	to run a single process per node, regardless of the settings
 *	selected at slurm_step_ctx_create time.
 *
 *	This is primarily used when launching 1 task per node as done
 * 	with IBM's PE where we want to launch a single pmd daemon
 *	on each node regardless of the number of tasks running on each
 *	node.
 * IN ctx - job step context generated by slurm_step_ctx_create
 * IN node_list - node list of nodes to run on
 * IN node_cnt - number of nodes to run on
 * IN/OUT curr_task_num - task_id of newest task, initialze to zero
 * RET SLURM_SUCCESS or SLURM_ERROR (with slurm_errno set)
 */
extern int slurm_step_ctx_daemon_per_node_hack(slurm_step_ctx_t *ctx,
					       char *node_list,
					       uint32_t node_cnt,
					       uint32_t *curr_task_num);

/*
 * slurm_step_ctx_destroy - free allocated memory for a job step context.
 * IN ctx - job step context generated by slurm_step_ctx_create
 * RET SLURM_SUCCESS or SLURM_ERROR (with slurm_errno set)
 */
extern int slurm_step_ctx_destroy(slurm_step_ctx_t *ctx);

#endif /* _STEP_CTX_H */
