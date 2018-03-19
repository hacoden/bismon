// file engine_BM.c

/***
    BISMON 
    Copyright © 2018 Basile Starynkevitch (working at CEA, LIST, France)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/


#include "bismon.h"
#include "engine_BM.const.h"

// expertise command_handler _5W4PPQFYdj2_3HYUlMsu3oZ

extern objrout_sigBM ROUTINEOBJNAME_BM (_5W4PPQFYdj2_3HYUlMsu3oZ);

value_tyBM
ROUTINEOBJNAME_BM (_5W4PPQFYdj2_3HYUlMsu3oZ)    //
(struct stackframe_stBM * stkf, //
 const value_tyBM arg1,         // the reciever
 const value_tyBM arg2,         // the comment or origin
 const value_tyBM arg3_ __attribute__ ((unused)),       //
 const value_tyBM arg4_ __attribute__ ((unused)),       //
 const quasinode_tyBM * restargs_ __attribute__ ((unused)))
{
  LOCALFRAME_BM (stkf, /*descr: */ BMK_5W4PPQFYdj2_3HYUlMsu3oZ,
                 value_tyBM argv;
                 objectval_tyBM * prevexpertiseob;
                 objectval_tyBM * thisexpertiseob;
                 value_tyBM resultv;
    );
  objectval_tyBM *k_current_expertise = BMK_1Lw70JO0z4y_7VFmnlVAN2U;
  objectval_tyBM *k_expertise = BMK_3BK1iRJQKks_9em7267pi4g;
  _.argv = arg1;
  if (isobject_BM (_.argv))
    {
      _.thisexpertiseob = _.argv;
      bool good = false;
      objlock_BM (_.thisexpertiseob);
      good = objectisinstance_BM (_.thisexpertiseob, k_expertise);
      objunlock_BM (_.thisexpertiseob);
      if (!good)
        {
          if (pthread_self () == mainthreadid_BM)
            {
              log_begin_message_BM ();
              log_puts_message_BM
                ("invalid argument to ,expertise command: ");
              log_object_message_BM (_.thisexpertiseob);
              log_end_message_BM ();
            };
          LOCALRETURN_BM (NULL);
        }
    }
  else if (isstring_BM (_.argv))
    {
      _.thisexpertiseob = makeobj_BM ();
      objputclass_BM (_.thisexpertiseob, k_expertise);
      objputspacenum_BM (_.thisexpertiseob, GlobalSp_BM);
      objputattr_BM (_.thisexpertiseob, BMP_comment, _.argv);
      if (pthread_self () == mainthreadid_BM)
        {
          log_begin_message_BM ();
          log_puts_message_BM (",expertise created ");
          log_object_message_BM (_.thisexpertiseob);
          log_printf_message_BM (" : %s", bytstring_BM (_.argv));
          log_end_message_BM ();
        };
    }
  else
    {
      if (pthread_self () == mainthreadid_BM)
        {
          log_begin_message_BM ();
          log_puts_message_BM
            ("bad argument to ,expertise command, object or comment-string expected");
          log_end_message_BM ();
        };
      LOCALRETURN_BM (NULL);
    }
  {
    objlock_BM (BMP_the_system);
    _.prevexpertiseob =
      objectcast_BM (objgetattr_BM (BMP_the_system, k_current_expertise));
    if (_.prevexpertiseob != _.thisexpertiseob)
      {
        objputattr_BM (BMP_the_system, k_current_expertise,
                       _.thisexpertiseob);
        objtouchnow_BM (BMP_the_system);
      }
    objunlock_BM (BMP_the_system);
  }
#warning incomplete expertise command_handler _5W4PPQFYdj2_3HYUlMsu3oZ
  WEAKASSERT_BM (false
                 &&
                 "incomplete expertise command_handler _5W4PPQFYdj2_3HYUlMsu3oZ");
  LOCALRETURN_BM (NULL);
}                               /* end expertise command_handler _5W4PPQFYdj2_3HYUlMsu3oZ */
